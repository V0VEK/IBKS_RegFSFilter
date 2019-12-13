#include <fltKernel.h>

#include "FsFilter.h"


NTSTATUS FsFilterStart(PDRIVER_OBJECT p_driver_object) {
	
	DEBUG_PRINT("Filesystem filter start successfull!");
	return STATUS_SUCCESS;
}


NTSTATUS FsFilterStop() {

	DEBUG_PRINT("Filesystem filter stop successfull!");
	return STATUS_SUCCESS;
}