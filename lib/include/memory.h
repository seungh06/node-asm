#ifndef MEMORY_H
#define MEMORY_H

#include <Windows.h>
#include <napi.h>

template <class type> type read_memory(HANDLE process, DWORD64 address);
bool read_string(HANDLE process, DWORD64 address, std::string* res);
Napi::Value EXPORT_READ_MEMORY(const Napi::CallbackInfo& arguments);

template <class type> void write_memory(HANDLE process, DWORD64 address, type value);
Napi::Value EXPORT_WRITE_MEMORY(const Napi::CallbackInfo& arguments);

Napi::Value EXPORT_GET_POINTER_ADDRESS(const Napi::CallbackInfo& arguments);

bool memory_compare(const unsigned char* data, const char* signature);
bool aob_scan(HANDLE process, uintptr_t base_address, uint8_t* module_bytes, DWORD module_size, const char* signature, uintptr_t offset, uintptr_t* address);
Napi::Value EXPORT_AOB_SCAN_MODULE(const Napi::CallbackInfo& arguments);

Napi::Value EXPORT_VIRTUAL_ALLOC(const Napi::CallbackInfo& arguments);

#endif