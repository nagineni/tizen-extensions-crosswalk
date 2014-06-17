// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include "package/package_extension.h"
#include "package/package_instance.h"

common::Extension* CreateExtension() {
  return new PackageExtension;
}

// JS source code for the API
extern const char kSource_package_api[];

PackageExtension::PackageExtension() {
  SetExtensionName("tizen.package");
  SetJavaScriptAPI(kSource_package_api);
}

PackageExtension::~PackageExtension() {}

common::Instance* PackageExtension::CreateInstance() {
  return new PackageInstance;
}
