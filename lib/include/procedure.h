#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <napi.h>

Napi::Value EXPORT_CALL_PROCEDURE_X86(const Napi::CallbackInfo& arguments);
Napi::Value EXPORT_CALL_PROCEDURE_X64(const Napi::CallbackInfo& arguments);

#endif