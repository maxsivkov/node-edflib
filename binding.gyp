{
  "targets": [
    {
      "target_name": "edflib",
      "sources": [ "src/edflib_wrapper.cc", "src/edflib_asyncops.cc", "src/edflib_111/edflib.cpp", "src/edflib_111/sine.cpp", "src/edflib_111/utils.cpp"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
	, "<!(node -e \"require('nan-check')\")"
	, "<!(node -e \"require('nan-marshal')\")"
	, "src/edflib_111"
      ]
      , "defines": ["_CRT_SECURE_NO_DEPRECATE"
        , "_CRT_NONSTDC_NO_DEPRECATE"
        , "_CRT_SECURE_NO_WARNINGS"
        , "_USE_MATH_DEFINES"
	]
    , "cflags": ["-std=c++11"]
    , "files" : ["src/edf_wrapper.h"]	
    , 'conditions': [
      ['OS=="win"', {
        'configurations': {
          'Debug': {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'RuntimeTypeInfo': 'true'
                , "ExceptionHandling": 1
              },
            }
          }, # Debug
          'Release': {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'RuntimeTypeInfo': 'true'
                , "ExceptionHandling": 1
              },
            },
          }, # Release
        }, # configurations
      } ], # OS=="win"
      [ 'OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
        {
            'cflags!': [ '-fno-exceptions','-fno-rtti']
            , 'cflags_cc!': [ '-fno-exceptions', '-fno-rtti']
        }
      ],
      [ 'OS=="mac"',
        {
            'xcode_settings': { 'GCC_ENABLE_CPP_RTTI': 'YES', 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' }
        }
      ]
    ], # conditions
    }
  ]
  
}
