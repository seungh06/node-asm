#include <napi.h>

#include "./include/process.h"
#include "./include/module.h"
#include "./include/memory.h"
#include "./include/procedure.h"

Napi::Object InitLibrary(Napi::Env environment, Napi::Object exports) {
        
        exports.Set(Napi::String::New(environment, "load_processes"), Napi::Function::New(environment, EXPORT_LOAD_PROCESSES));
        exports.Set(Napi::String::New(environment, "open_process")  , Napi::Function::New(environment, EXPORT_OPEN_PROCESS  ));
        exports.Set(Napi::String::New(environment, "close_handle")  , Napi::Function::New(environment, EXPORT_CLOSE_HANDLE  ));
        exports.Set(Napi::String::New(environment, "is_x86")        , Napi::Function::New(environment, EXPORT_IS_X86        ));

        exports.Set(Napi::String::New(environment, "load_modules") , Napi::Function::New(environment, EXPORT_LOAD_MODULES ));
        exports.Set(Napi::String::New(environment, "inject_module"), Napi::Function::New(environment, EXPORT_INJECT_MODULE));
        exports.Set(Napi::String::New(environment, "eject_module") , Napi::Function::New(environment, EXPORT_EJECT_MODULE ));

        exports.Set(Napi::String::New(environment, "read_memory")        , Napi::Function::New(environment, EXPORT_READ_MEMORY        ));
        exports.Set(Napi::String::New(environment, "write_memory")       , Napi::Function::New(environment, EXPORT_WRITE_MEMORY       ));
        exports.Set(Napi::String::New(environment, "get_pointer_address"), Napi::Function::New(environment, EXPORT_GET_POINTER_ADDRESS));
        exports.Set(Napi::String::New(environment, "aob_scan")           , Napi::Function::New(environment, EXPORT_AOB_SCAN_MODULE    ));
        exports.Set(Napi::String::New(environment, "virtual_alloc")      , Napi::Function::New(environment, EXPORT_VIRTUAL_ALLOC      ));

        exports.Set(Napi::String::New(environment, "call_procedure_x86"), Napi::Function::New(environment, EXPORT_CALL_PROCEDURE_X86));
        exports.Set(Napi::String::New(environment, "call_procedure_x64"), Napi::Function::New(environment, EXPORT_CALL_PROCEDURE_X64));

        return exports;
}

NODE_API_MODULE(asm, InitLibrary);