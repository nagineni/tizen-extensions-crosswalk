// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "package/package_manager.h"

#include <pkgmgr-info.h>
#include <package-manager.h>
#include <package_manager.h>

#include "common/picojson.h"

PackageManager::PackageManager() {
  if (package_manager_request_create(&request_handle_) !=
                                    PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager could not be created" << std::endl;
    return;
  }
}

PackageManager::~PackageManager() {}

void PackageManager::PackageManagerDestroy() {
  if (package_manager_request_destroy(request_handle_) !=
                                    PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager could not be destroyed" << std::endl;
    return;
  }
}

int PackageManager::Install(const char *path, int *id) {
  if (package_manager_request_install(request_handle_, path, id) !=
                                      PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "Install error" << std::endl;
    return PACKAGE_MANAGER_ERROR_INVALID_PARAMETER;
  } else {
    // success
    return PACKAGE_MANAGER_ERROR_NONE;
  }
}

int PackageManager::UnInstall(const char *name, int *id) {
  if (package_manager_request_uninstall(request_handle_, name, id) !=
                                      PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "UnInstall error" << std::endl;
  return PACKAGE_MANAGER_ERROR_INVALID_PARAMETER;
  } else {
    // success
    return PACKAGE_MANAGER_ERROR_NONE;
  }
}

int PackageManager::GetPackageInfo(const char *package_id) {
  package_info_h package_info;
  int ret = package_manager_get_package_info(package_id, &package_info);
  if (!ret) {
    // Success
    std::cout << "PackageInfo :->" << std::endl;

  } else {
    // Error
    std::cerr << "PackageInfo ERROR" << std::endl;
  }
  return ret;
}
