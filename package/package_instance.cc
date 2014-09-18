// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "package/package_instance.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include "common/picojson.h"


#include "package/package_info.h"
#include "package/package_manager.h"

PackageInstance::PackageInstance() {
  package_manager_.reset(new PackageManager(this));
}

PackageInstance::~PackageInstance() {
  package_manager_->PackageManagerDestroy();
}

void PackageInstance::HandleMessage(const char* message) {
  printf("PackageInstance::HandleMessage===>\n");
  picojson::value v;
  picojson::value::object o;

  std::string err;
  picojson::parse(v, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cerr << "Ignoring message. \n";
    return;
  }
#ifdef DEBUG_JSON
  std::cout << "HandleMessage: " << message << std::endl;
#endif
  std::string cmd = v.get("cmd").to_str();
  if (cmd == "PackageManager.install") {
    HandleInstallRequest(v);
  } else if (cmd == "PackageManager.uninstall") {
    HandleUninstallRequest(v);
  } else if (cmd == "PackageManager.getPackagesInfo") {
    HandleGetPackagesInfoRequest(v);
  } else if (cmd == "PackageManager.getPackageInfo") {
    HandleGetPackageInfoRequest(v);
  } else {
    std::cerr << "Message " + cmd + " is not supported.\n";
  }
}

void PackageInstance::PostAsyncReply(const picojson::value& msg,
    picojson::value::object& reply) {
  printf("PackageInstance::PostostAsyncReply==>\n");
  reply["replyId"] = picojson::value(msg.get("replyId").get<double>());
  picojson::value v(reply);
  PostMessage(v.serialize().c_str());
}

void PackageInstance::PostAsyncErrorReply(const picojson::value& msg,
    WebApiAPIErrors error_code, const std::string& error_msg = "") {
  printf("PackageInstance::PostAsyncErrorReply==>\n");
  picojson::value::object reply;
  reply["isError"] = picojson::value(true);
  reply["errorCode"] = picojson::value(static_cast<double>(error_code));
  if (!error_msg.empty())
    reply["errorMessage"] = picojson::value(error_msg.c_str());
  PostAsyncReply(msg, reply);
}

void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg,
    picojson::value::object& reply) {
  printf("PackageInstance::PostAsyncSuccessReply======= 00000\n");

  reply["isError"] = picojson::value(false);
  reply["replyId"] = picojson::value(msg.get("replyId").get<double>());
  //reply["id"] = picojson::value(msg.get("id"));
  //reply["cmd"] = picojson::value("OnComplete");

  picojson::value v(reply);
  PostMessage(v.serialize().c_str());
}

void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg) {
  printf("PackageInstance::PostAsyncSuccessReply=======\n");
  picojson::value::object reply;
  PostAsyncSuccessReply(msg, reply);
}

/*void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg,
    picojson::value& value) {
  printf("PackageInstance::PostAsyncSuccessReply======= NICEEEEEEEEEEEEEEEEEE\n");
  picojson::value::object reply;
  reply["onprogress"] = picojson::value("OnProgress");;
  reply["progress"] = value;
  PostAsyncSuccessReply(msg, reply);
}*/

void PackageInstance::HandleSyncMessage(const char* message) {
  picojson::value v;
  picojson::value::object o;

  std::string err;
  picojson::parse(v, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cerr << "Ignoring message.\n";
    return;
  }
#ifdef DEBUG_JSON_CMD
  std::cout << "HandleSyncMessage: " << message << std::endl;
#endif
  std::string cmd = v.get("cmd").to_str();

  if (cmd == "PackageManager.setPackageInfoEventListener") {
    package_manager_->SetEventListener();
  } else if (cmd == "PackageManager.unsetPackageInfoEventListener") {
    package_manager_->UnsetEventListener();
  } else {
    std::cerr << "Message " + cmd + " is not supported.\n";
  }

#ifdef DEBUG_JSON_CMD
  std::cout << "Reply: " << v.serialize().c_str() << std::endl;
#endif
  SendSyncReply(v.serialize().c_str());
}

int PackageInstance::CallbackReplyId(int request_id) {
  printf("reuqest_id ===>: %d\n", request_id);
  std::map<int, double>::iterator iter = callbacks_id_map_.find(request_id);
  if (iter != callbacks_id_map_.end())
    return iter->second;

  return 0;
}

void PackageInstance::OnRequestHandled(int id, const char *packageid,
    int event_type, int event_state, int progress, int error) {

  double reply_id = CallbackReplyId(id);

  if (!error || event_state != FAILED) {
    printf("PackageInstance::OnRequestHandled===: %d\n", event_state);
    // success
    picojson::value::object reply;
    reply["isError"] = picojson::value(false);
    reply["replyId"] = picojson::value(reply_id);

    reply["id"] = picojson::value(packageid);
    if (event_state != COMPLETED) {
      reply["progress"] = picojson::value(static_cast<double>(progress));
    }
    reply["cmd"] = picojson::value(static_cast<double>(event_state));

    picojson::value v(reply);
    PostMessage(v.serialize().c_str());
  } else{
    printf("PackageInstance::OnRequestHandled=== Error\n");
    // failed
    picojson::value::object o;
    o["_reply_id"] = picojson::value(reply_id);
    picojson::value msg(o);
    PostAsyncErrorReply(msg, WebApiAPIErrors::UNKNOWN_ERR, "UnInstall Error");
  }
}

void PackageInstance::HandleInstallRequest(const picojson::value& msg) {
  const char* path = msg.get("package_path").to_str().c_str();
  printf("PackageInstance::HandleInstallRequest===: %s\n", path);

  int id;
  int ret = package_manager_->Install(path, &id);

  if (ret) {
    // failed
    std::cerr << "Install Error:" << "'\n";
    PostAsyncErrorReply(msg, WebApiAPIErrors::UNKNOWN_ERR, "Install Error");
  } else {
    callbacks_id_map_[id] = msg.get("replyId").get<double>();
  }
}

void PackageInstance::HandleUninstallRequest(const picojson::value& msg) {
  const char* name = msg.get("package_id").to_str().c_str();

  int id;
  int ret = package_manager_->UnInstall(name, &id);

  callbacks_id_map_[id] = msg.get("replyId").get<double>();

  if (ret) {
    // failed
    std::cerr << "UnInstall Error:" << "'\n";
    PostAsyncErrorReply(msg, WebApiAPIErrors::UNKNOWN_ERR, "UnInstall Error");
  } else {
    callbacks_id_map_[id] = msg.get("replyId").get<double>();
  }
}

void PackageInstance::HandleGetPackageInfoRequest(const picojson::value& msg) {
  const char* package_id = msg.get("package_id").to_str().c_str();
  int ret = package_manager_->GetPackageInfo(package_id);
}

void PackageInstance::HandleGetPackagesInfoRequest(const picojson::value& msg) {
  // TODO(riju): Not implemented yet
}
