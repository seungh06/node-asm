#ifndef MODULE_H
#define MODULE_H

#include <Windows.h>
#include <TlHelp32.h>
#include <napi.h>

std::vector<MODULEENTRY32> load_modules(DWORD process_id);
Napi::Value EXPORT_LOAD_MODULES(const Napi::CallbackInfo& arguments);

Napi::Value EXPORT_INJECT_MODULE(const Napi::CallbackInfo& arguments);
Napi::Value EXPORT_EJECT_MODULE(const Napi::CallbackInfo& arguments);
#endif