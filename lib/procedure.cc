#include <Windows.h>
#include <TlHelp32.h>
#include <napi.h>
#include <iostream>

Napi::Value EXPORT_CALL_PROCEDURE_X86(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        
        HANDLE      process   = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64     procedure = arguments.operator[](1).As<Napi::Number>().Int64Value();
        Napi::Array args      = arguments.operator[](2).As<Napi::Array>();
        BOOL        dump      = arguments.operator[](3).As<Napi::Boolean>().Value();

        std::vector<unsigned char> argument_shellcode;
        for(unsigned int index = 0; index < args.Length(); index ++) {
                Napi::Object argument      = args.Get(index).As<Napi::Object>();
                std::string  argument_type = argument.Get("type").As<Napi::String>().Utf8Value();

                if(!argument_type.compare("string")) {
                        std::string data = argument.Get("value").As<Napi::String>().Utf8Value();

                        void* allocated = VirtualAllocEx(process, NULL, data.length(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                        WriteProcessMemory(process, allocated, data.c_str(), data.length(), NULL);
                        
                        argument_shellcode.push_back(0x68);
                        for(int index = 0; index < 4; index ++) 
                                argument_shellcode.push_back(((uintptr_t) allocated >> (index * 8)) & 0xFF);
                }

                else if(argument_type.find("int8") != std::string::npos || !argument_type.compare("bool")) {
                        int data = !argument_type.compare("bool")
                                ? argument.Get("value").As<Napi::Boolean>().Value() ? 1 : 0
                                : argument.Get("value").As<Napi::Number>().Int32Value();

                        argument_shellcode.push_back(0x6A);
                        argument_shellcode.push_back(data);
                }

                else if(argument_type.find("int") != std::string::npos || !argument_type.compare("float")) {
                        int data = argument.Get("value").As<Napi::Number>().Int32Value();

                        argument_shellcode.push_back(0x68);
                        for(int index = 0; index < 4; index ++) 
                                argument_shellcode.push_back((data >> (index * 8)) & 0xFF);
                }
        }

        std::vector<unsigned char> shellcode = {
                0xE8, 0x00, 0x00, 0x00, 0x00,    // 0xE8 : call    0x00000000
                0xC3                             // 0xC3 : ret
        };
        shellcode.insert(shellcode.begin(), argument_shellcode.begin(), argument_shellcode.end());

        void* allocated = VirtualAllocEx(process, NULL, shellcode.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        DWORD64 procedure_address = procedure - (uintptr_t) allocated - (argument_shellcode.size() + 5);

        for(int index = 0; index < 4; index ++)
                shellcode.at(argument_shellcode.size() + 1 + index) = (procedure_address >> (index * 8)) & 0xFF;
        WriteProcessMemory(process, allocated, shellcode.data(), shellcode.size(), NULL);
        
        if(dump) {
                for(size_t index = 0; index < shellcode.size(); index ++)
                        std::cout << std::hex << (0xFF & shellcode[index]) << (index == shellcode.size() - 1 ? '\n' : ' ');
        }
        
        HANDLE thread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE) allocated, 0, 0, 0);
        WaitForSingleObject(thread, INFINITE);

        DWORD exitcode = 0;
        GetExitCodeThread(thread, &exitcode);

        VirtualFreeEx(process, allocated, 0, MEM_RELEASE);
        return Napi::Number::New(environment, exitcode);
}

Napi::Value EXPORT_CALL_PROCEDURE_X64(const Napi::CallbackInfo& arguments) {
        Napi::Env environment = arguments.Env();
        
        HANDLE      process   = (HANDLE) arguments.operator[](0).As<Napi::Number>().Int64Value();
        DWORD64     procedure = arguments.operator[](1).As<Napi::Number>().Int64Value();
        Napi::Array args      = arguments.operator[](2).As<Napi::Array>();
        BOOL        dump      = arguments.operator[](3).As<Napi::Boolean>().Value();

        unsigned char argument_size = args.Length() * 8;
        std::vector<unsigned char> argument_shellcode = {
                0x55,                              // push rbp
                0x48, 0x8B, 0xEC,                  // mov  rbp, rsp
                0x48, 0x83, 0xEC, argument_size    // sub  rsp, [ size ]
        }; 
        
        for(unsigned char index = 0; index < args.Length(); index ++) {
                Napi::Object argument      = args.Get(index).As<Napi::Object>();
                std::string  argument_type = argument.Get("type").As<Napi::String>().Utf8Value();

                if(!argument_type.compare("string")) {
                        std::string data = argument.Get("value").As<Napi::String>().Utf8Value();

                        void* allocated = VirtualAllocEx(process, NULL, data.length(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                        WriteProcessMemory(process, allocated, data.c_str(), data.length(), NULL);
                        
                        argument_shellcode.push_back(index > 1 && index < 4 ? 0x49 : 0x48); // movabs = 0x48(RCX, RDX, RAX), 0x49(R8, R9)
                        argument_shellcode.push_back(
                                  index == 0 ? 0xB9 // RCX
                                : index == 1 ? 0xBA // RDX
                                : index == 2 ? 0xB8 // R8
                                : index == 3 ? 0xB9 // R9
                                : 0xB8              // RAX
                        );

                        for(int index = 0; index < 8; index ++) 
                                argument_shellcode.push_back(((uintptr_t) allocated >> (index * 8)) & 0xFF);

                        if(index > 3) argument_shellcode.push_back(0x50); // push rax
                } 

                else if(!argument_type.compare("int64")) {
                        DWORD64 data = argument.Get("value").As<Napi::Number>().Int64Value();
                        
                        argument_shellcode.push_back(index > 1 && index < 4 ? 0x49 : 0x48); // movabs = 0x48(RCX, RDX, RAX), 0x49(R8, R9)
                        argument_shellcode.push_back(
                                index == 0   ? 0xB9 // RCX
                                : index == 1 ? 0xBA // RDX
                                : index == 2 ? 0xB8 // R8
                                : index == 3 ? 0xB9 // R9
                                : 0xB8              // RAX
                        );

                        for(int index = 0; index < 8; index ++) 
                                argument_shellcode.push_back((data >> (index * 8)) & 0xFF);
                        
                        if(index > 3) argument_shellcode.push_back(0x50); // push rax
                }

                else if(argument_type.find("int") != std::string::npos || !argument_type.compare("bool") || !argument_type.compare("float")) {
                        int data = !argument_type.compare("bool")
                                ? argument.Get("value").As<Napi::Boolean>().Value() ? 1 : 0
                                : argument.Get("value").As<Napi::Number>().Int32Value();

                        bool is_single = argument_type.find("int8") != std::string::npos || !argument_type.compare("bool");
                        argument_shellcode.push_back(
                                index < 2 ? 0x48 :
                                index < 4 ? 0x49 :
                                is_single ? 0x6A : 0x68
                        ); // movabs = 0x48(RCX, RDX), 0x49(R8, R9) / push = 0x6A(imm8), 0x68(imm32);

                        if(index < 4) {
                                argument_shellcode.push_back(0xC7);
                                argument_shellcode.push_back(
                                        index == 0   ? 0xC1 // RCX
                                        : index == 1 ? 0xC2 // RDX
                                        : index == 2 ? 0xC0 // R8
                                        : 0xC1              // R9
                                );
                        }

                        if(index < 4 || !is_single) {
                                for(int index = 0; index < 4; index ++)
                                        argument_shellcode.push_back((data >> (index* 8)) & 0xFF);
                        } else argument_shellcode.push_back(data); // push data
                }
        }

        std::vector<unsigned char> shellcode = {
                0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // mov     rax, 0x0000000000000000
                0xFF, 0xD0,                                                    // call    rax
                0x48, 0x83, 0xC4, argument_size,                               // add     rsp, [ size ] 
                0x48, 0x8b, 0xE5,                                              // mov     rsp, rbp
                0x5D,                                                          // pop     rbp
                0xC3                                                           // ret
        };
        shellcode.insert(shellcode.begin(), argument_shellcode.begin(), argument_shellcode.end());

        void* allocated = VirtualAllocEx(process, NULL, shellcode.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        for(int index = 0; index < 8; index ++) 
                shellcode.at(argument_shellcode.size() + 2 + index) = (procedure >> (index * 8)) & 0xFF;
        WriteProcessMemory(process, allocated, shellcode.data(), shellcode.size(), NULL);

        if(dump) {
                for(size_t index = 0; index < shellcode.size(); index ++)
                        std::cout << std::hex << (0xFF & shellcode[index]) << (index == shellcode.size() - 1 ? '\n' : ' ');
        }
        
        HANDLE thread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE) allocated, 0, 0, 0);
        WaitForSingleObject(thread, INFINITE);

        DWORD exitcode = 0;
        GetExitCodeThread(thread, &exitcode);

        VirtualFreeEx(process, allocated, 0, MEM_RELEASE);
        return Napi::Number::New(environment, exitcode);
}