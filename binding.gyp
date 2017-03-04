{
  "targets": [
    {
      "target_name": "cppaddon",
      "sources": [ "cpp_addon.cc" ],
      "include_dirs" : [
		      "<!(node -e \"require('nan')\")", 
		      "<!(node -e \"require('streaming-worker-sdk')\")"
		  ],
       "cflags": ["-Wall", "-std=c++11"],
      "cflags!": [ '-fno-exceptions' ],
      "cflags_cc!": [ '-fno-exceptions' ],
      "conditions": [
        [ 'OS!="win"', {
          "cflags+": [ "-std=c++11" ],
          "cflags_c+": [ "-std=c++11" ],
          "cflags_cc+": [ "-std=c++11" ],
        }],
        [ 'OS=="mac"', {
          "xcode_settings": {
            "OTHER_CPLUSPLUSFLAGS" : [ "-std=c++11", "-stdlib=libc++" ],
            "OTHER_LDFLAGS": [ "-stdlib=libc++" ],
            "MACOSX_DEPLOYMENT_TARGET": "10.7"
          },
        }],
      ]
    }
  ]
}