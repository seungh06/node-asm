#include <Windows.h>
#include <TlHelp32.h>
#include <napi.h>

std::vector<MODULEENTRY32> load_modules(DWORD process_id) {
        std::vector<MODULEENTRY32> modules;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
        
        MODULEENTRY32 mod;
        mod.dwSize = sizeof(mod);

        if(Module32First(snapshot, &mod)) {
                do {
                        modules.push_back(mod);
                } while(Module32Next(snapshot, &mod));
        }

        CloseHandle(snapshot);
        return modules;
}

Napi::Value EXPORT_LOAD_MODULES(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        std::vector<MODULEENTRY32> modules = load_modules(arguments.operator[](0).As<Napi::Number>().Int32Value());
        
        Napi::Object exports = Napi::Array::New(environment, modules.size());
        for(size_t index = 0; index < modules.size(); index ++) {
                Napi::Object mod = Napi::Object::New(environment);
                
                mod.Set(Napi::String::New(environment, "name")   , Napi::String::New(environment, (char*) modules[index].szModule));
                mod.Set(Napi::String::New(environment, "path")   , Napi::String::New(environment, (char*) modules[index].szExePath));
                mod.Set(Napi::String::New(environment, "pid")    , Napi::Value::From(environment, (int) modules[index].th32ProcessID));
                mod.Set(Napi::String::New(environment, "address"), Napi::Value::From(environment, (uintptr_t) modules[index].modBaseAddr));
                mod.Set(Napi::String::New(environment, "size")   , Napi::Value::From(environment, (int) modules[index].modBaseSize));

                exports.Set(index, mod);
        }
        return exports;
}

Napi::Value EXPORT_INJECT_MODULE(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE      handle    = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        std::string path      = arguments.operator[](1).As<Napi::String>().Utf8Value();
        DWORD64     procedure = arguments.operator[](2).As<Napi::Number>().Int64Value();

        void* allocated = VirtualAllocEx(handle, NULL, path.length() + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        WriteProcessMemory(handle, allocated, path.c_str(), path.length() + 1, NULL);

        HANDLE thread = CreateRemoteThread(handle, NULL, NULL, (LPTHREAD_START_ROUTINE) procedure, allocated, NULL, NULL);
        WaitForSingleObject(thread, INFINITE);
        
        DWORD exitcode = 0;
        GetExitCodeThread(thread, &exitcode);

        VirtualFreeEx(handle, allocated, 0, MEM_RELEASE);
        CloseHandle(thread);

        return Napi::Boolean::New(environment, exitcode > 0);
}

Napi::Value EXPORT_EJECT_MODULE(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE  handle    = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64 address   = arguments.operator[](1).As<Napi::Number>().Int64Value();
        DWORD64 procedure = arguments.operator[](2).As<Napi::Number>().Int64Value();

        HANDLE thread = CreateRemoteThread(handle, NULL, NULL, (LPTHREAD_START_ROUTINE) procedure, (LPVOID) address, NULL, NULL);
        WaitForSingleObject(thread, INFINITE);
        
        DWORD exitcode = 0;
        GetExitCodeThread(thread, &exitcode);
        CloseHandle(thread);

        return Napi::Boolean::New(environment, exitcode != 0);
}