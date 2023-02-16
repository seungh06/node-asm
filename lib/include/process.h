#ifndef PROCESS_H
#define PROCESS_H

#include <Windows.h>
#include <TlHelp32.h>
#include <napi.h>

std::vector<PROCESSENTRY32> load_processes();
BOOL is_x86(HANDLE process);

Napi::Value EXPORT_LOAD_PROCESSES(const Napi::CallbackInfo& arguments);
Napi::Value EXPORT_OPEN_PROCESS(const Napi::CallbackInfo& arguments);
Napi::Value EXPORT_CLOSE_HANDLE(const Napi::CallbackInfo& arguments);
Napi::Value EXPORT_IS_X86(const Napi::CallbackInfo& arguments);

#endif