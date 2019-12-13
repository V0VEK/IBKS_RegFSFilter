#include <fltKernel.h>
#include <Ntddk.h>
#include <Ntstrsafe.h>

#include "Logger.h"
#include "FsFilter.h"
#include "RegFilter.h"
#include "ConfigParser.h"


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

// TODO: maybe need fix
VOID Clean() {
	FsFilterStop();
	RegFilterStop();
}

VOID DriverUnload(PDRIVER_OBJECT p_driver_object)
{
	UNREFERENCED_PARAMETER(p_driver_object);
	PAGED_CODE();

	NTSTATUS status = FsFilterStop();
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("FS filter stop failure");
	}

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

	NTSTATUS status = FsFilterStart(p_driver_object);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("FS filter start failure");
		return -1; // Maybe need fix
	}

	status = RegFilterStart(p_driver_object);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Registry filter start failure");
		Clean();
		return -1; // Maybe need fix
	}

	DEBUG_PRINT("FS filter and registry filter currently running");

	DEBUG_PRINT("Start reading protected entities list in registry key: %wZ", p_registry_path);
	status = ReadProtectedEntitiesList(p_registry_path);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Registry key reading error");
		Clean();
		return -1;
	}


	DEBUG_PRINT("Driver successfuly loaded!");
	return STATUS_SUCCESS;
}

