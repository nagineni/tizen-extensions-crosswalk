{
  'includes':[
    '../common/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'tizen_package',
      'type': 'loadable_module',
      'variables': {
        'packages': [
          'capi-appfw-package-manager'
        ],
      },
      'sources': [
        'package_api.js',
        'package_extension.cc',
        'package_extension.h',
        'package_info.h',
        'package_instance.cc',
        'package_instance.h',
        'package_manager.cc',
        'package_manager.h',
      ],
      'includes': [
        '../common/pkg-config.gypi',
      ],
    }
  ]
}
