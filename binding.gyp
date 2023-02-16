{
    "targets": [
        {
            "target_name": "asm",
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
            "conditions": [
                ['OS == "win"', {
                    "sources": [ "lib/asm.cc", "lib/process.cc", "lib/module.cc", "lib/memory.cc", "lib/procedure.cc" ]
                }],
                
                ['OS != "win"', {}],
            ]
        }
    ]
}