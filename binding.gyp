{
  'targets': [
    {
      'target_name': 'device-info',
      'cflags_cc!':[
          '-fno-exceptions',
          '-fno-rtti',
      ],
      'sources': [
        'src/device-info.cc'
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")'
      ],
      'conditions': [
        ['OS=="win"', {
          'defines': [
            '__WIN32=true'
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1
            }
          }
        }],
        ['OS=="linux"', {
          'defines': [
            '__LINUX=true'
          ]
        }],
        ['OS=="mac"', {
          'defines': [
            '__DARWIN=true'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'OTHER_CFLAGS': [
              '-Wall',
              '-Wno-missing-braces',
              '-Wno-format-security',
              '-std=c++11',
              '-ObjC++',
              '-mmacosx-version-min=10.7'
            ]
          },
          'libraries': [
              '-lobjc'
          ],
        }]
      ]
    }
  ]
}
