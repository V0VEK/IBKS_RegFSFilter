#pragma once

#include <fltKernel.h>

#include "Logger.h"


NTSTATUS RegFilterStart(
	PDRIVER_OBJECT p_driver_object
);

NTSTATUS RegFilterStop(

);


//____________________________________________________
// For COM Port
#define COMM_PORT_NAME RTL_CONSTANT_STRING(L"\\IBKS_REG_Filter_COM_Port")

//____________________________________________________