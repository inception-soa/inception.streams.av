{
    "includes": ["common.gypi"],
    "targets": [
        {
            "target_name": "nav",
            "sources": [
              "src/nav.cpp",
              "src/nav_types.cpp",
              "src/nav_transcoder.cpp"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(node -e \"require('streaming-worker-sdk')\")",
                "<!(pwd)/deps/libav"
            ],
            "cflags!": [ '-fno-exceptions' ],
            "cflags_cc!": [ '-fno-exceptions' ],
            "conditions":[
                ['OS=="mac"', {
                    "cflags": [
                        "-g",
                        "-D__STDC_CONSTANT_MACROS",
                        "-D_FILE_OFFSET_BITS=64",
                        "-D_LARGEFILE_SOURCE",
                        "-O3",
                        "-Wall",
                        "-std=c++11"],
                    "libraries": [
                        "-framework CoreFoundation",
                        "-framework CoreVideo",
                        "-framework VideoDecodeAcceleration",
                        "-lm",
                        "-lpthread",
                        "-lavcodec",
                        "-lavdevice",
                        "-lavfilter",
                        "-lavformat",
                        "-lavresample",
                        "-lavutil",
                        "-lswscale",
                        "-L<!(pwd)/deps/libav/libavcodec",
                        "-L<!(pwd)/deps/libav/libavdevice",
                        "-L<!(pwd)/deps/libav/libavfilter",
                        "-L<!(pwd)/deps/libav/libavformat",
                        "-L<!(pwd)/deps/libav/libavresample",
                        "-L<!(pwd)/deps/libav/libavutil",
                        "-L<!(pwd)/deps/libav/libswscale"
                    ],
                    "xcode_settings": {
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        "MACOSX_DEPLOYMENT_TARGET": "10.11"
                    }
                }]
            ]
        }
    ],
}
