// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGE_PACKAGE_INSTANCE_H_
#define PACKAGE_PACKAGE_INSTANCE_H_

#include <string>
#include <algorithm>
#include <vector>
#include <memory>


#include "common/extension.h"
#include "common/picojson.h"
#include "tizen/tizen.h"

namespace picojson {
class value;
}

class PackageInformation;
class PackageManager;

typedef long long int64; // NOLINT

class PackageInstance : public common::Instance {
 public:
  PackageInstance();
  virtual ~PackageInstance();

  PackageManager* package_manager() { return package_manager_.get(); }


  void OnRequestHandled(int id, const char *packageid,
      int event_type, int event_state, int progress, int error);

 private:
  enum PackageEventState {
    STARTED = 0,
    PROCESSING = 1,
    COMPLETED = 2,
    FAILED = 3,
  };

  // common::Instance implementation
  virtual void HandleMessage(const char* msg);
  virtual void HandleSyncMessage(const char* msg);

  void HandleInstallRequest(const picojson::value& json);
  void HandleUninstallRequest(const picojson::value& json);
  void HandleGetPackageInfoRequest(const picojson::value& json);
  void HandleGetPackagesInfoRequest(const picojson::value& json);

  // Asynchronous message helpers
  void PostAsyncReply(const picojson::value& msg,
                      picojson::value::object& value);
  void PostAsyncErrorReply(const picojson::value&, WebApiAPIErrors,
                          const std::string& error_msg);
  void PostAsyncSuccessReply(const picojson::value&, picojson::value::object&);
  void PostAsyncSuccessReply(const picojson::value&, picojson::value&);
  void PostAsyncSuccessReply(const picojson::value&, WebApiAPIErrors);
  void PostAsyncSuccessReply(const picojson::value&);
  int CallbackReplyId(int requiest_id);

  // Tizen CAPI helpers
  std::unique_ptr<PackageManager> package_manager_;

  // Map JS reply_id to a C API callback
  std::map<int, double> callbacks_id_map_;

//  typedef std::map<int, int> RequestIdToThreadId;
//  RequestIdToThreadId request_id_map_;
};

#endif  // PACKAGE_PACKAGE_INSTANCE_H_
