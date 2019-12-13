#pragma once

#include <fltKernel.h>

#include "Logger.h"


NTSTATUS RegFilterStart(
	PDRIVER_OBJECT p_driver_object
);

NTSTATUS RegFilterStop(

);