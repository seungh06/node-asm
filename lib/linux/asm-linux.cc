#include <napi.h>
#include <iostream>

Napi::Value EXPORT_WRITE(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        std::string data = arguments.operator[](0).As<Napi::String>().Utf8Value();
        std::cout << data << std::endl;

        return environment.Null();
}

Napi::Object InitLibrary(Napi::Env environment, Napi::Object exports) {
        exports.Set(Napi::String::New(environment, "write"), Napi::Function::New(environment, EXPORT_WRITE));
        return exports;
}

NODE_API_MODULE(asm, InitLibrary);