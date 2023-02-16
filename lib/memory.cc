#include <Windows.h>
#include <napi.h>

#include "./include/process.h"

template <class type> type read_memory(HANDLE process, DWORD64 address) {
        type res;
        ReadProcessMemory(process, (LPVOID) address, &res, sizeof(res), NULL);

        return res;
}

bool read_string(HANDLE process, DWORD64 address, std::string* res) {
        int length = 0, batch_size = 256;
        char* data = (char*) malloc(sizeof(char) * batch_size);

        while (length <= batch_size * 4096) {
                bool success = ReadProcessMemory(process, (LPVOID) (address + length), data, batch_size, NULL);
                if (success == 0) break;

                for (const char* pointer = data; pointer - data < batch_size; ++ pointer) {
                        if (*pointer != '\0') continue;
                        length += pointer - data + 1;

                        char* buffer = (char*) malloc(length);
                        ReadProcessMemory(process, (LPVOID) address, buffer, length, NULL);

                        *res = std::string(buffer);
                        free(data), free(buffer);
                        return true;
                }
                length += batch_size;
        }

        free(data);
        return false;
}

template <class type> void write_memory(HANDLE process, DWORD64 address, type value) {
        WriteProcessMemory(process, (LPVOID) address, &value, sizeof(type), NULL);
}

bool memory_compare(const unsigned char* data, const char* signature) {
        for(; *signature; *signature != ' ' ? ++ data : data, ++ signature) {
                if(*signature == ' ' || *signature == '?') {
                        continue;
                }

                if(*data != ((
                        (signature[0] >= '0' && signature[0] <= '9') ? (signature[0] - '0') : (( signature[0] & (~0x20)) - 'A' + 0xa)
                ) << 4 | (
                        (signature[1] >= '0' && signature[1] <= '9') ? (signature[1] - '0') : (( signature[1] & (~0x20)) - 'A' + 0xa)
                ))) {
                        return false;
                }

                ++ signature;
        }
        return true;
}

bool aob_scan(HANDLE process, uintptr_t base_address, uint8_t* module_bytes, DWORD module_size, const char* signature, uintptr_t offset, uintptr_t* address) {
        auto   max_offset = module_size - 0x1000; 
        SIZE_T size       = is_x86(process) ? 4 : 8;

        for(uintptr_t index = 0; index < max_offset; ++ index) {
                if(!memory_compare(module_bytes + index, signature)) continue;

                uintptr_t res = base_address + index + offset;
                ReadProcessMemory(process, (LPVOID) res, &res, size, nullptr);
                *address = res;

                return true;
        }
        return false;
}

Napi::Value EXPORT_READ_MEMORY(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        
        HANDLE      handle       = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64     address      = arguments.operator[](1).As<Napi::Number>().Int64Value();
        std::string defined_type = arguments.operator[](2).As<Napi::String>().Utf8Value();
        Napi::Value return_value = environment.Null();

        if(!defined_type.compare("int8")) {
                int8_t result = read_memory<int8_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        }

        else if (!defined_type.compare("uint8")) {
                uint8_t result = read_memory<uint8_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("int16")) {
                int16_t result = read_memory<int16_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("uint16")) {
                uint16_t result = read_memory<uint16_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("int32")) {
                int32_t result = read_memory<int32_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("uint32")) {
                uint32_t result = read_memory<uint32_t>(handle, address);
                return_value = Napi::Value::From(environment, result);
        }

        else if(!defined_type.compare("int64")) {
                int64_t result = read_memory<int64_t>(handle, address);
                return_value = Napi::Value::From(environment, Napi::BigInt::New(environment, result));
        } 
        
        else if(!defined_type.compare("uint64")) {
                uint64_t result = read_memory<uint64_t>(handle, address);
                return_value = Napi::Value::From(environment, Napi::BigInt::New(environment, result));
        } 
        
        else if(!defined_type.compare("float")) {
                float result = read_memory<float>(handle, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("double")) {
                double result = read_memory<double>(handle, address);
                return_value = Napi::Value::From(environment, result); 
        } 
        
        else if(!defined_type.compare("bool")) {
                bool result = read_memory<bool>(handle, address);
                return_value = Napi::Boolean::From(environment, result);
        }

        else if(!defined_type.compare("string")) {
                std::string result;
                if(read_string(handle, address, &result)){
                        return_value = Napi::String::New(environment, result);
                } else Napi::Error::New(environment, "cannot read memory as string data.").ThrowAsJavaScriptException();
        }

        return return_value;
}

Napi::Value EXPORT_WRITE_MEMORY(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE      handle       = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64     address      = arguments.operator[](1).As<Napi::Number>().Int64Value();
        std::string defined_type = arguments.operator[](3).As<Napi::String>().Utf8Value();

        Napi::Value data = arguments.operator[](2);
        if(!defined_type.compare("int8")) {
                write_memory<int8_t>(handle, address, data.As<Napi::Number>().Int32Value());
        } 
  
        else if(!defined_type.compare("uint8")) {
                write_memory<uint8_t>(handle, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int16")) {
                write_memory<int16_t>(handle, address, data.As<Napi::Number>().Int32Value());
        } 
        
        else if(!defined_type.compare("uint16")) {
                write_memory<uint16_t>(handle, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int32")) {
                write_memory<int32_t>(handle, address, data.As<Napi::Number>().Int32Value());
        } 
        
        else if(!defined_type.compare("uint32")) {
                write_memory<uint32_t>(handle, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int64")) {
                bool lossless;
                write_memory<int64_t>(handle, address, data.As<Napi::BigInt>().Int64Value(&lossless));
        } 
        
        else if(!defined_type.compare("uint64")) {
                bool lossless;
                write_memory<uint64_t>(handle, address, data.As<Napi::BigInt>().Uint64Value(&lossless));
        } 
        
        else if(!defined_type.compare("float")) {
                write_memory<float>(handle, address, data.As<Napi::Number>().FloatValue());
        } 

        else if(!defined_type.compare("double")) {
                write_memory<float>(handle, address, data.As<Napi::Number>().DoubleValue());
        } 
        
        else if(!defined_type.compare("bool")) {
                write_memory<bool>(handle, address, data.As<Napi::Boolean>().Value());
        }
        
        else if(defined_type.compare("string") == 0) {
                write_memory<std::string>(handle, address, data.As<Napi::String>().Utf8Value());
        }

        return environment.Null();
}

Napi::Value EXPORT_GET_POINTER_ADDRESS(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE      handle       = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64     base_address = arguments.operator[](1).As<Napi::Number>().Int64Value();
        Napi::Array offsets      = arguments.operator[](2).As<Napi::Array>();

        SIZE_T size = is_x86(handle) ? 4 : 8;
        
        uintptr_t address;
        ReadProcessMemory(handle, (LPVOID) base_address, &address, size, NULL);

        for(unsigned int index = 0; index < offsets.Length(); index ++) {
                address += offsets.Get(index).As<Napi::Number>().Int32Value();
                if(index != offsets.Length() - 1) {
                        ReadProcessMemory(handle, (LPVOID) address, &address, size, NULL);
                }
        }

        return Napi::Number::New(environment, address);
}

Napi::Value EXPORT_AOB_SCAN_MODULE(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE  handle       = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64 base_address = arguments.operator[](1).As<Napi::Number>().Int64Value();
        DWORD   module_size  = arguments.operator[](2).As<Napi::Number>().Int32Value();

        std::string signature = arguments.operator[](3).As<Napi::String>().Utf8Value();
        DWORD       offset    = arguments.operator[](4).As<Napi::Number>().Int32Value();
        DWORD       extra     = arguments.operator[](5).As<Napi::Number>().Int32Value();

        uint8_t* module_bytes = new uint8_t[module_size];
        ReadProcessMemory(handle, (LPVOID) base_address, module_bytes, module_size, NULL);

        uintptr_t address = 0;
        aob_scan(handle, (uintptr_t) base_address, module_bytes, module_size, signature.c_str(), offset, &address);
        
        return address == 0 ? environment.Null() : Napi::Value::From(environment, address + extra);
}

Napi::Value EXPORT_VIRTUAL_ALLOC(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        HANDLE process         = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        void*  address         = (void *) arguments.operator[](1).As<Napi::Number>().Int64Value();
        SIZE_T size            = arguments.operator[](2).As<Napi::Number>().Int64Value();
        DWORD  allocation_type = arguments.operator[](3).As<Napi::Number>().Uint32Value();
        DWORD  protection      = arguments.operator[](4).As<Napi::Number>().Uint32Value();

        void* allocated = VirtualAllocEx(process, address, size, allocation_type, protection);
        return Napi::Value::From(environment, (intptr_t) allocated);
}