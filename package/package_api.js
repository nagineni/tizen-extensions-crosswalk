// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var PackageEventState = {
  'STARTED': 0,
  'PROCESSING': 1,
  'COMPLETED': 2,
  'FAILED': 3
};

var _callbacks = {};
var _nextReplyId = 0;

function getNextReplyId() {
  return _nextReplyId++;
}

function postMessage(msg, callback) {
  console.log("postMessage===========");
  var replyId = getNextReplyId();
  _callbacks[replyId] = callback;
  msg.replyId = replyId;
  console.log("postMessage===========" + msg.replyId);
  extension.postMessage(JSON.stringify(msg));
  console.log("postMessage=========== Done");
}

function sendSyncMessage(msg) {
  return extension.internal.sendSyncMessage(JSON.stringify(msg));
}

extension.setMessageListener(function(msg) {
  var m = JSON.parse(msg);
  var replyId = m.replyId;
  var callback = _callbacks[replyId];

  console.log("extension.setMessageListener");

/*  if (replyId == 0) { // replyId zero is for events
  console.log("No reply ID SO EVENT");
    if (exports.changeListener != null) {
      if (m.eventType == 'INSTALLED')
        exports.changeListener.oninstalled(m.name);
      else if (m.eventType == 'UPDATED')
        exports.changeListener.onupdated(m.name);
      else if (m.eventType == 'UNINSTALLED')
        exports.changeListener.onuninstalled(m.name);
    }
  } else */
  if (typeof(callback) === 'function') {
    console.log("extension.setMessageListeneri====" + m.cmd);
    callback(m);
    if (m.isError || m.cmd == PackageEventState.COMPLETED || m.cmd == PackageEventState.FAILED) {
      delete m.replyId;
      delete _callbacks[replyId];
    }
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
  this.changeListener = null;
}

PackageManager.prototype.install = function(path, onsuccess, onerror) {
  console.log("PackageManager.prototype.install.");
/*  if (!xwalk.utils.validateArguments('s?ff', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);*/

  console.log("PackageManager.prototype.install.000 ");
  postMessage({
    cmd: 'PackageManager.install',
    package_path: path
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    } else if (result.cmd == PackageEventState.COMPLETED) {
      console.log("Completeddd===========================\n");
      if (typeof onsuccess.oncomplete !== 'undefined')
        onsuccess.oncomplete(result.id);
    } else {
      console.log("Progresssssssssssssssssssssssssssssss\n");
      if (typeof onsuccess.onprogress !== 'undefined')
        onsuccess.onprogress(result.id, result.progress);
    }
  });
};

PackageManager.prototype.uninstall = function(id, onsuccess, onerror) {
  console.log("PackageManager.prototype.uninstall.");
  if (!xwalk.utils.validateArguments('s?of', arguments))
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);
  console.log("PackageManager.prototype.uninstall.000");

  postMessage({
    cmd: 'PackageManager.uninstall',
    package_id: id
  }, function(result) {
      console.log('00000 UnInstall completed for package id->', id);
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    } else if (result.cmd == PackageEventState.COMPLETED) {
      console.log("Completeddd===========================\n");
      if (typeof onsuccess.oncomplete !== 'undefined')
        onsuccess.oncomplete(result.id);
    } else {
      console.log("Progresssssssssssssssssssssssssssssss" + result.id + result.progress);
      if (typeof onsuccess.onprogress !== 'undefined')
        onsuccess.onprogress(result.id, result.progress);
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
    package_id: id
  }, function(result) {
    if (result.isError) {
      if (onerror)
        onerror(new tizen.WebAPIError(result.errorCode));
    }
  });
};

PackageManager.prototype.setPackageInfoEventListener = function(onchange) {
  if (!xwalk.utils.validateArguments('o', arguments)) {
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);
  }

  if (!xwalk.utils.validateObject(listener, 'fff',
      ['oninstalled', 'onupdated', 'onuninstalled'])) {
    throw new tizen.WebAPIException(tizen.WebAPIException.TYPE_MISMATCH_ERR);
  }

  this.changeListener = listener;
  sendSyncMessage({cmd: 'PackageManager.setPackageInfoEventListener'});
};

PackageManager.prototype.unsetPackageInfoEventListener = function() {
  this.changeListener = null;
  sendSyncMessage({cmd: 'PackageManager.unsetPackageInfoEventListener'});
};

exports = new PackageManager();
