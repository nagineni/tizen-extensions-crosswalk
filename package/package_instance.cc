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
  package_manager_.reset(new PackageManager());
}

PackageInstance::~PackageInstance() {
  package_manager_->PackageManagerDestroy();
}

void PackageInstance::HandleMessage(const char* message) {
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
  reply["replyId"] = picojson::value(msg.get("replyId").get<double>());
  picojson::value v(reply);
  PostMessage(v.serialize().c_str());
}

void PackageInstance::PostAsyncErrorReply(const picojson::value& msg,
    WebApiAPIErrors error_code, const std::string& error_msg = "") {
  picojson::value::object reply;
  reply["isError"] = picojson::value(true);
  reply["errorCode"] = picojson::value(static_cast<double>(error_code));
  if (!error_msg.empty())
    reply["errorMessage"] = picojson::value(error_msg.c_str());
  PostAsyncReply(msg, reply);
}

void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg,
    picojson::value::object& reply) {
  reply["isError"] = picojson::value(false);
  reply["replyId"] = picojson::value(msg.get("replyId").get<double>());

  picojson::value v(reply);
  PostMessage(v.serialize().c_str());
}

void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg) {
  picojson::value::object reply;
  PostAsyncSuccessReply(msg, reply);
}

void PackageInstance::PostAsyncSuccessReply(const picojson::value& msg,
    picojson::value& value) {
  picojson::value::object reply;
  reply["value"] = value;
  PostAsyncSuccessReply(msg, reply);
}

void PackageInstance::HandleSyncMessage(const char* message) {
}

void PackageInstance::HandleInstallRequest(const picojson::value& msg) {
  const char* path = msg.get("path").to_str().c_str();
  int id = std::stoi(msg.get("id").to_str());

  int ret = package_manager_->Install(path, &id);
  if (!ret) {
    // success
    picojson::object obj;
    obj["id"] = msg.get("id");
    PostAsyncSuccessReply(picojson::value(obj));
  } else {
    // failed
    std::cerr << "Install Error:" << "'\n";
    PostAsyncErrorReply(msg, WebApiAPIErrors::UNKNOWN_ERR, "Install Error");
  }
}

void PackageInstance::HandleUninstallRequest(const picojson::value& msg) {
  const char* name = msg.get("name").to_str().c_str();
  int id = std::stoi(msg.get("id").to_str());
  int ret = package_manager_->UnInstall(name, &id);
  if (!ret) {
    // success
    picojson::object obj;
    obj["id"] = msg.get("id");
    PostAsyncSuccessReply(picojson::value(obj));
  } else {
    // failed
    std::cerr << "UnInstall Error:" << "'\n";
    PostAsyncErrorReply(msg, WebApiAPIErrors::UNKNOWN_ERR, "UnInstall Error");
  }
}

void PackageInstance::HandleGetPackageInfoRequest(const picojson::value& msg) {
  const char* package_id = msg.get("package_id").to_str().c_str();
  int ret = package_manager_->GetPackageInfo(package_id);
}

void PackageInstance::HandleGetPackagesInfoRequest(const picojson::value& msg) {
  // TODO(riju): Not implemented yet
}
