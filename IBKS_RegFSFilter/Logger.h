#pragma once
#include <fltKernel.h>

#define PREFIX "[filter] "
#define DEBUG_PRINT(...)       KdPrint((PREFIX __VA_ARGS__));
#define ERROR_PRINT(msg, code) KdPrint((PREFIX msg " {code=%08x}", code));