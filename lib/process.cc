#include <Windows.h>
#include <TlHelp32.h>
#include <napi.h>

std::vector<PROCESSENTRY32> load_processes() {
        std::vector<PROCESSENTRY32> processes;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        PROCESSENTRY32 process;
        process.dwSize = sizeof(process);

        if (Process32First(snapshot, &process)) {
                do {
                        processes.push_back(process);
                } while(Process32Next(snapshot, &process));
        }

        CloseHandle(snapshot);
        return processes;
}

BOOL is_x86(HANDLE process) {
        BOOL res; 
        IsWow64Process(process, &res);

        return res;
}


Napi::Value EXPORT_LOAD_PROCESSES(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        std::vector<PROCESSENTRY32> processes = load_processes();

        Napi::Array exports = Napi::Array::New(environment, processes.size());
        for(size_t index = 0; index < processes.size(); index ++) {
                Napi::Object process = Napi::Object::New(environment);

                process.Set(Napi::String::New(environment, "exe")        , Napi::String::New(environment, (char *) processes[index].szExeFile));
                process.Set(Napi::String::New(environment, "pid")        , Napi::Value::From(environment, (int) processes[index].th32ProcessID));
                process.Set(Napi::String::New(environment, "ppid")       , Napi::Value::From(environment, (int) processes[index].th32ParentProcessID));
                process.Set(Napi::String::New(environment, "dw_size")    , Napi::Value::From(environment, (int) processes[index].dwSize));
                process.Set(Napi::String::New(environment, "classbase")  , Napi::Value::From(environment, (int) processes[index].pcPriClassBase));
                process.Set(Napi::String::New(environment, "cnt_threads"), Napi::Value::From(environment, (int) processes[index].cntThreads));

                exports.Set(index, process);
        }

        return exports;
}

Napi::Value EXPORT_OPEN_PROCESS(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        DWORD process_id = arguments.operator[](0).As<Napi::Number>().Uint32Value();
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);

        return handle == NULL ? environment.Null() : Napi::Value::From(environment, (uintptr_t) handle);
}

Napi::Value EXPORT_CLOSE_HANDLE(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE handle = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        return Napi::Boolean::New(environment, CloseHandle(handle));
}

Napi::Value EXPORT_IS_X86(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE handle = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        return Napi::Boolean::New(environment, is_x86(handle));
}