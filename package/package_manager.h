// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGE_PACKAGE_MANAGER_H_
#define PACKAGE_PACKAGE_MANAGER_H_

#include <package_manager.h>

#include <string>

class PackageInformation;

class PackageManager {
 public:
  PackageManager();
  ~PackageManager();
  void PackageManagerDestroy();

  int Install(const char *path, int *id);
  int UnInstall(const char *name, int *id);
  int GetPackageInfo(const char *package_id);

 private:
  package_manager_request_h request_handle_;
};

#endif  // PACKAGE_PACKAGE_MANAGER_H_
