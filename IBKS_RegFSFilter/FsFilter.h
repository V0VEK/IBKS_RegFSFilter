#pragma once

#include <fltKernel.h>

#include "Logger.h"
#include "ConfigParser.h"


NTSTATUS FsFilterStart(
	PDRIVER_OBJECT p_driver_object
);

NTSTATUS FsFilterStop(FLT_FILTER_UNLOAD_FLAGS Flags);




//____________________________________________________
// For COM Port
#define COMM_PORT_NAME_FS RTL_CONSTANT_STRING(L"\\IBKS_FS_Filter_COM_Port")

PFLT_PORT g_Port_FS;
PFLT_PORT g_ClientPort_FS;
//____________________________________________________