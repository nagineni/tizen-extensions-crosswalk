// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "package/package_manager.h"

#include <pkgmgr-info.h>
#include <package-manager.h>
#include <package_manager.h>
#include "package/package_instance.h"

#include "common/picojson.h"

void PackageRequestEventCallback(
    int id,
    const char *type,
    const char *package,
    package_manager_event_type_e event_type,
    package_manager_event_state_e event_state,
    int progress,
    package_manager_error_e error,
    void *user_data) {
  printf("ID:%d, type:%s, Pakckage: %s, event_type: %d, event_state: %d, progress: %d, error: %d", id, type, package, event_type, event_state, progress, error);

  PackageManager* manager = static_cast<PackageManager*> (user_data);
  PackageInstance* instance = manager->package_instance();
  if (!instance)
    return;

   instance->OnRequestHandled(id, package, event_type, event_state, progress, error);
}


PackageManager::PackageManager(PackageInstance* instace)
    : instance_(instace) {
  printf("PackageManager::PackageManager========\n");

  if (package_manager_create(&manager_) !=
                             PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager creation failed." << std::endl;
  }

  if (package_manager_request_create(&request_handle_) !=
                                     PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager fail to create the request handle." << std::endl;
  } else {
    package_manager_request_set_mode(request_handle_, PACKAGE_MANAGER_REQUEST_MODE_QUIET);

    package_manager_request_set_event_cb(request_handle_,
        PackageRequestEventCallback, this);
  }
}

void PackageManagerEventCallback(
    const char *type,
    const char *package,
    package_manager_event_type_e event_type,
    package_manager_event_state_e event_state,
    int progress,
    package_manager_error_e error,
    void *user_data) {
  printf("type:%s, Pakckage: %s, event_type: %d, event_state: %d, progress: %d, error: %d", type, package, event_type, event_state, progress, error);
}


PackageManager::~PackageManager() {
  printf("PackageManager::PackageManager======== Final\n");
}

void PackageManager::PackageManagerDestroy() {
  printf("PackageManager::PackageManager=======Destroy=\n");
  if (package_manager_request_unset_event_cb(request_handle_) !=
       PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager fail to unregisters the callback." << std::endl;
  }

  if (package_manager_request_destroy(request_handle_) !=
                                      PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager fail to destroy the request handle." << std::endl;
  }

  if (package_manager_destroy(manager_) !=
                              PACKAGE_MANAGER_ERROR_NONE) {
    std::cerr << "package manager destroy failed." << std::endl;
  }
}

int PackageManager::Install(const char *path, int *id) {
  if (package_manager_request_install(request_handle_, path, id) !=
                                      PACKAGE_MANAGER_ERROR_NONE) {
    printf("PackageManager::Install=======================error: %s\n", path);
    std::cerr << "Install error" << std::endl;
    return PACKAGE_MANAGER_ERROR_INVALID_PARAMETER;
  } else {
    printf("PackageManager::Install======================= no error\n");
    // success
    return PACKAGE_MANAGER_ERROR_NONE;
  }
}

int PackageManager::UnInstall(const char *name, int *id) {
    printf("PackageManager::UnInstall=======================: %s\n", name);

  if (package_manager_request_uninstall(request_handle_, name, id) !=
                                      PACKAGE_MANAGER_ERROR_NONE) {
    printf("PackageManager::UnInstall=======================error\n");
    std::cerr << "UnInstall error" << std::endl;
  return PACKAGE_MANAGER_ERROR_INVALID_PARAMETER;
  } else {
    printf("PackageManager::UnInstall=======================no error\n");
    // success
    return PACKAGE_MANAGER_ERROR_NONE;
  }
}

int PackageManager::GetPackageInfo(const char *package_id) {
  package_info_h package_info;

  return package_manager_get_package_info(package_id, &package_info);
}

int PackageManager::SetEventListener() {
  return package_manager_set_event_cb(manager_, PackageManagerEventCallback, this);
}

int PackageManager::UnsetEventListener() {
  return package_manager_unset_event_cb(manager_);
}
