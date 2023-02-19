#include <napi.h>
#include <sys/uio.h>

template <class type> type read_memory(uint32_t process, void * address) {
        type res;

        struct iovec local[1];
        local[0].iov_base = &res;
        local[0].iov_len  = sizeof(res);

        struct iovec remote[1];
        remote[0].iov_base = address;
        remote[0].iov_len  = sizeof(res);

        process_vm_readv(process, local, 1, remote, 1, 0);

        return res;
}

template <class type> void write_memory(uint32_t process, void* address, type value) {
        struct iovec local[1];
        local[0].iov_base = &value;
        local[0].iov_len = sizeof(value);

        struct iovec remote[1];
        remote[0].iov_base = address;
        remote[0].iov_len = sizeof(value);

        process_vm_writev(process, local, 1, remote, 1, 0);
}

Napi::Value EXPORT_READ_MEMORY(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        
        uint32_t    process_id   = arguments.operator[](0).As<Napi::Number>().Uint32Value();
        void *      address      = (void *) arguments.operator[](1).As<Napi::Number>().Int64Value();
        std::string defined_type = arguments.operator[](2).As<Napi::String>().Utf8Value();

        Napi::Value return_value = environment.Null();

        if(!defined_type.compare("int8")) {
                int8_t result = read_memory<int8_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        }

        else if (!defined_type.compare("uint8")) {
                uint8_t result = read_memory<uint8_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("int16")) {
                int16_t result = read_memory<int16_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("uint16")) {
                uint16_t result = read_memory<uint16_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("int32")) {
                int32_t result = read_memory<int32_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("uint32")) {
                uint32_t result = read_memory<uint32_t>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        }

        else if(!defined_type.compare("int64")) {
                int64_t result = read_memory<int64_t>(process_id, address);
                return_value = Napi::Value::From(environment, Napi::BigInt::New(environment, result));
        } 
        
        else if(!defined_type.compare("uint64")) {
                uint64_t result = read_memory<uint64_t>(process_id, address);
                return_value = Napi::Value::From(environment, Napi::BigInt::New(environment, result));
        } 
        
        else if(!defined_type.compare("float")) {
                float result = read_memory<float>(process_id, address);
                return_value = Napi::Value::From(environment, result);
        } 
        
        else if(!defined_type.compare("double")) {
                double result = read_memory<double>(process_id, address);
                return_value = Napi::Value::From(environment, result); 
        } 
        
        else if(!defined_type.compare("bool")) {
                bool result = read_memory<bool>(process_id, address);
                return_value = Napi::Boolean::From(environment, result);
        }

        else if(!defined_type.compare("string")) {

        }

        return return_value;
}

Napi::Value EXPORT_WRITE_MEMORY(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();

        uint32_t    process_id   = arguments.operator[](0).As<Napi::Number>().Uint32Value();
        void *      address      = (void *) arguments.operator[](1).As<Napi::Number>().Int64Value();
        std::string defined_type = arguments.operator[](3).As<Napi::String>().Utf8Value();

        Napi::Value data = arguments.operator[](2);
        if(!defined_type.compare("int8")) {
                write_memory<int8_t>(process_id, address, data.As<Napi::Number>().Int32Value());
        } 
  
        else if(!defined_type.compare("uint8")) {
                write_memory<uint8_t>(process_id, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int16")) {
                write_memory<int16_t>(process_id, address, data.As<Napi::Number>().Int32Value());
        } 
        
        else if(!defined_type.compare("uint16")) {
                write_memory<uint16_t>(process_id, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int32")) {
                write_memory<int32_t>(process_id, address, data.As<Napi::Number>().Int32Value());
        } 
        
        else if(!defined_type.compare("uint32")) {
                write_memory<uint32_t>(process_id, address, data.As<Napi::Number>().Uint32Value());
        } 
        
        else if(!defined_type.compare("int64")) {
                bool lossless;
                write_memory<int64_t>(process_id, address, data.As<Napi::BigInt>().Int64Value(&lossless));
        } 
        
        else if(!defined_type.compare("uint64")) {
                bool lossless;
                write_memory<uint64_t>(process_id, address, data.As<Napi::BigInt>().Uint64Value(&lossless));
        } 
        
        else if(!defined_type.compare("float")) {
                write_memory<float>(process_id, address, data.As<Napi::Number>().FloatValue());
        } 

        else if(!defined_type.compare("double")) {
                write_memory<float>(process_id, address, data.As<Napi::Number>().DoubleValue());
        } 
        
        else if(!defined_type.compare("bool")) {
                write_memory<bool>(process_id, address, data.As<Napi::Boolean>().Value());
        }
        
        else if(defined_type.compare("string") == 0) {
                write_memory<std::string>(process_id, address, data.As<Napi::String>().Utf8Value());
        }

        return environment.Null();
}

Napi::Object InitLibrary(Napi::Env environment, Napi::Object exports) {
        
        exports.Set(Napi::String::New(environment, "read_memory") , Napi::Function::New(environment, EXPORT_READ_MEMORY ));
        exports.Set(Napi::String::New(environment, "write_memory"), Napi::Function::New(environment, EXPORT_WRITE_MEMORY));

        return exports;
}

NODE_API_MODULE(asm, InitLibrary);