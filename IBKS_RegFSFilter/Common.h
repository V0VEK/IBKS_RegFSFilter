#pragma once
#include <wdm.h>
#include <ntstrsafe.h>

#define MY_POOL_TAG '1gaT'


NTSTATUS WriteDataToUnicodeString(PUNICODE_STRING dest, PVOID source);
PVOID CreateBuffer(size_t size);