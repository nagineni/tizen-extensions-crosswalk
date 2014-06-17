// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGE_PACKAGE_INFO_H_
#define PACKAGE_PACKAGE_INFO_H_

#include <string>
#include <vector>

class PackageInformation {
 public:
  PackageInformation() {}
  PackageInformation(std::string id,
            std::string name,
            std::string iconPath,
            std::string version,
            long totalSize, // NOLINT
            long dataSize, // NOLINT
            std::string lastModified,
            std::string author,
            std::string description,
            std::vector<std::string> appIds)
      : id_(id), name_(name), iconPath_(iconPath), version_(version),
        totalSize_(totalSize), dataSize_(dataSize), lastModified_(lastModified),
        author_(author), description_(description), appIds_(appIds) {}

  ~PackageInformation() {}

  // Getters & Getters
  const std::string& id() const { return id_; }
  void setId(const std::string& id) { id_ = id; }
  const std::string& name() const { return name_; }
  void setName(const std::string& name) { name_ = name; }
  const std::string& iconPath() const { return iconPath_; }
  void setIconPath(const std::string& iconPath) { iconPath_ = iconPath; }
  const std::string& version() const { return version_; }
  void setVersion(const std::string& version) { version_ = version; }
  const long& totalSize() const { return totalSize_; } // NOLINT
  void setTotalSize(const long& totalSize) { totalSize_ = totalSize; } // NOLINT
  const long& dataSize() const { return dataSize_; } // NOLINT
  void setDataSize(const long& dataSize) { dataSize_ = dataSize; } // NOLINT
  const std::string& lastModified() const { return lastModified_; }
  void setLastModified(const std::string& lastModified) {
    lastModified_ = lastModified; }
  const std::string& author() const { return author_; }
  void setAuthor(const std::string& author) { author_ = author; }
  const std::string& description() const { return description_; }
  void setDescription(const std::string& description) {
    description_ = description; }
  const std::vector<std::string>& appIds() const { return appIds_; }
  void setAppIds(const std::vector<std::string>& appIds) {
    appIds_ = appIds; }

#ifdef DEBUG_PACKAGE
  void print(void);
#endif

 private:
  std::string id_;
  std::string name_;
  std::string iconPath_;
  std::string version_;
  long totalSize_; // NOLINT
  long dataSize_; // NOLINT
  std::string lastModified_;
  std::string author_;
  std::string description_;
  std::vector<std::string> appIds_;
};

#endif  // PACKAGE_PACKAGE_INFO_H_
