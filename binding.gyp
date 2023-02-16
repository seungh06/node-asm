{
    "targets": [
        {
            "target_name": "asm",
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "sources": [ 
                "lib/asm.cc",
                "lib/process.cc",
                "lib/module.cc",
                "lib/memory.cc",
                "lib/procedure.cc"
            ],
            "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
        }
    ]
}