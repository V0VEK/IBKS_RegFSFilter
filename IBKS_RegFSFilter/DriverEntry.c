#include <fltKernel.h>
#include <Ntddk.h>
#include <Ntstrsafe.h>

#include "Logger.h"
#include "FsFilter.h"
#include "RegFilter.h"
#include "ConfigParser.h"


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

VOID DriverUnload(PDRIVER_OBJECT p_driver_object)
{
	UNREFERENCED_PARAMETER(p_driver_object);
	PAGED_CODE();

	DeleteProtectedEntitiesList();
	NTSTATUS status;

	status = RegFilterStop();
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Registry filter stop failure");
	}

	DEBUG_PRINT("STOP");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver_object, PUNICODE_STRING p_registry_path)
{
	UNREFERENCED_PARAMETER(p_registry_path);
	p_driver_object->DriverUnload = DriverUnload;
	DEBUG_PRINT("IBKS Filter starting...");


	DEBUG_PRINT("Start reading protected entities list in registry key: %wZ", REG_PATH_FOR_ANTIVIRUS);

	NTSTATUS status = ReadProtectedEntitiesList();

	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("ReadProtectedEntitiesList ERROR!");
		return status;
	}

	status = FsFilterStart(p_driver_object);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("FS filter start failure");
		return status;
	}

	status = RegFilterStart(p_driver_object);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Registry filter start failure");
		return status;
	}

	DEBUG_PRINT("FS filter and registry filter currently running");

	DEBUG_PRINT("Driver successfuly loaded!");
	return STATUS_SUCCESS;
}

