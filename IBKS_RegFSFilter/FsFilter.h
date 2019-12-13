#pragma once

#include <fltKernel.h>

#include "Logger.h"


NTSTATUS FsFilterStart(
	PDRIVER_OBJECT p_driver_object
);

NTSTATUS FsFilterStop(

);