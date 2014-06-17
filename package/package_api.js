// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var _callbacks = {};
var _nextReplyId = 0;

function getNextReplyId() {
  return _nextReplyId++;
}

function postMessage(msg, callback) {
  var replyId = getNextReplyId();
  _callbacks[replyId] = callback;
  msg.replyId = replyId;
  extension.postMessage(JSON.stringify(msg));
}

function sendSyncMessage(msg) {
  return extension.internal.sendSyncMessage(JSON.stringify(msg));
}

extension.setMessageListener(function(msg) {
  var m = JSON.parse(msg);
  var replyId = m.replyId;
  var callback = _callbacks[replyId];

  if (typeof(callback) === 'function') {
    callback(m);
    delete m.replyId;
    delete _callbacks[replyId];
  } else {
    console.log('Invalid replyId from Tizen Package API: ' + replyId);
  }
});

function PackageInformation(packageId, name, iconPath, version, totalSize,
                            dataSize, lastModified, author, description, appIds) {
  Object.defineProperties(this, {
    'packageId': { writable: false, value: packageId, enumerable: true },
    'name': { writable: false, value: name, enumerable: true },
    'iconPath': { writable: false, value: iconPath, enumerable: true },
    'version': { writable: false, value: version, enumerable: true },
    'totalSize': { writable: false, value: totalSize, enumerable: true },
    'dataSize': { writable: false, value: dataSize, enumerable: true },
    'lastModified': { writable: false, value: lastModified, enumerable: true },
    'author': { writable: false, value: author, enumerable: true },
    'description': { writable: false, value: description, enumerable: true },
    'appIds': { writable: false, value: appIds, enumerable: true }
  });
}

function PackageManager() {
}

PackageManager.prototype.install = function(path, onsuccess, onerror) {
  if (!xwalk.utils.validateArguments('s?ff', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);

  postMessage({
    cmd: 'PackageManager.install',
    path: path
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    } else if (onprogress) {
      // TODO(riju)
      // PackageInformationEventCallback
      var id = result.id;
      var progress = result.progress;
      console.log(progress + ' % Installed ; package id->', id);
    }
    else if (oncomplete) {
      var id = result.id;
      console.log('Install completed for package id->', id);
    }
  });
};

PackageManager.prototype.uninstall = function(id, onsuccess, onerror) {
  if (!xwalk.utils.validateArguments('s?ff', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);

  postMessage({
    cmd: 'PackageManager.uninstall',
    id: id
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    } else if (onprogress) {
      // TODO(riju)
      // PackageInformationEventCallback
      var id = result.id;
      var progress = result.progress;
      console.log(progress + ' % UnInstalled ; package id->', id);
    }
    else if (oncomplete) {
      var id = result.id;
      console.log('UnInstall completed for package id->', id);
    }
  });
};

PackageManager.prototype.getPackagesInfo = function(onsuccess, onerror) {
  if (!xwalk.utils.validateArguments('f?f', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);

  postMessage({
    cmd: 'PackageManager.getPackagesInfo'
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    } else if (onsuccess) {
      // TODO(riju)
      // PackageInformationArraySuccessCallback
      var packagesInfo;

      for (var i = 0; i < results.value.length; i++) {
        var packageInfo = result.value[i];
        var jsonPackageInfo = new PackageInformation(
            packageInfo.id,
            packageInfo.name,
            packageInfo.iconPath,
            packageInfo.version,
            packageInfo.totalSize,
            packageInfo.dataSize,
            packageInfo.lastModified,
            packageInfo.author,
            packageInfo.description,
            packageInfo.appIds);
        packagesInfo.push(jsonPackageInfo);
      }
      onsuccess(packagesInfo);
    }
  });
};

PackageManager.prototype.getPackageInfo = function(id) {
  if (!xwalk.utils.validateArguments('s', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);

  postMessage({
    cmd: 'PackageManager.getPackageInfo',
    id: id
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    }
  });
};

PackageManager.prototype.setPackageInfoEventListener = function(onchange) {
  if (!xwalk.utils.validateArguments('f', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);
};

PackageManager.prototype.unsetPackageInfoEventListener = function() {
};

exports = new PackageManager();
