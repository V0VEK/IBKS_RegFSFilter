#pragma once

#include <fltKernel.h>

#include "Logger.h"
#include "ConfigParser.h"


NTSTATUS FsFilterStart(
	PDRIVER_OBJECT p_driver_object
);

NTSTATUS FsFilterStop(FLT_FILTER_UNLOAD_FLAGS Flags);