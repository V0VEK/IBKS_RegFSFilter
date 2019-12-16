#pragma once
#include <wdm.h>
#include <ntstrsafe.h>


NTSTATUS WriteDataToUnicodeString(PUNICODE_STRING dest, PVOID source);
PVOID CreateBuffer(size_t size);