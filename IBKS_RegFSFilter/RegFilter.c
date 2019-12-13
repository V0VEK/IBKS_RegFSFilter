#include <fltKernel.h>

#include "RegFilter.h"


NTSTATUS RegFilterStart() {

	DEBUG_PRINT("Registry filter start successful!");
	return STATUS_SUCCESS;
}

NTSTATUS RegFilterStop() {

	DEBUG_PRINT("Registry filter stop successful!");
	return STATUS_SUCCESS;
}