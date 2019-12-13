#include "ConfigParser.h"


NTSTATUS ReadProtectedEntitiesList(PUNICODE_STRING regPath) {
	OBJECT_ATTRIBUTES oa;
	HANDLE hKey;

	InitializeObjectAttributes(&oa, regPath, 0, NULL, NULL);
	NTSTATUS status = ZwOpenKey(&hKey, KEY_READ, &oa);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Open key error. Key path: '%wZ'", regPath);
		return -1;
	}

	DEBUG_PRINT("Maybe this is key name: %wZ\nDirectory: %wZ\n", oa.ObjectName, oa.RootDirectory);

	status = ZwClose(hKey);
	if (!NT_SUCCESS(status)) {
		DEBUG_PRINT("Close key error. Key path: '%wZ'", regPath);
		return -1;
	}

	return STATUS_SUCCESS;
}