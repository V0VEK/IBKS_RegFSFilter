// Unreferenced formal parameter
#pragma warning( disable : 4100)

#include "ConfigParser.h"


RTL_QUERY_REGISTRY_ROUTINE RtlQueryRegistryRoutine;

NTSTATUS RtlQueryRegistryRoutine(
	PWSTR ValueName,
	ULONG ValueType,
	PVOID ValueData,
	ULONG ValueLength,
	PVOID Context,
	PVOID EntryContext
) {
	NTSTATUS status;

	UNICODE_STRING valueName;
	status = WriteDataToUnicodeString(&valueName, ValueName);
	if (status != STATUS_SUCCESS) {
		return status;
	}
	DEBUG_PRINT("Value Name: %wZ", valueName);


	UNICODE_STRING valueData;
	status = WriteDataToUnicodeString(&valueData, ValueData);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	if (RtlEqualUnicodeString(&valueName, &g_hardcodedStrForFS, TRUE)) {
		DEBUG_PRINT("Fill list for filesystem");

		status = FillListOfProtectedEntities(g_FS_Entities, &valueData);
		if (status != STATUS_SUCCESS) {
			return status;
		}
	}

	if (RtlEqualUnicodeString(&valueName, &g_hardcodedStrForRegistry, TRUE)) {
		DEBUG_PRINT("Fill list for registry");
		status = FillListOfProtectedEntities(g_Registry_Entities, &valueData);
		if (status != STATUS_SUCCESS) {
			return status;
		}
	}

	return STATUS_SUCCESS;
}


NTSTATUS FillListOfProtectedEntities(protectedEntity* list, PUNICODE_STRING data) {
	protectedEntity* node = list;
	protectedEntity* tmp = node;

	tmp = CreateBuffer(sizeof(protectedEntity));
	if (tmp == NULL) {
		return -1;
	}
	
	// Calculate all length of entities
	USHORT curEntityLength = 0;
	for (int i = 0; i < data->Length; i++) {
		curEntityLength++;
		if (data->Buffer[i] == L';') {
			tmp->path.Length = curEntityLength;
			tmp->path.MaximumLength = curEntityLength;

			node->Next = tmp;
			node = node->Next;

			tmp = CreateBuffer(sizeof(protectedEntity));
			if (tmp == NULL) {
				return -1;
			}
		}
	}

	// Fill list with entities
	node = list;
	WCHAR* str = CreateBuffer(node->path.Length * sizeof(WCHAR));
	if (str == NULL) {
		return -1;
	}
	int bufferCtr = 0;

	for (int i = 0; i < data->Length; i++) {
		str[bufferCtr++] = data->Buffer[i];
		if (data->Buffer[i] == L';') {
			str[bufferCtr - 1] = L'\0';
			RtlInitUnicodeString(&node->path, str);
			str = CreateBuffer(node->path.Length * sizeof(WCHAR));
			if (str == NULL) {
				return -1;
			}
			DEBUG_PRINT("Written buffer: %wZ", node->path);

			node = node->Next;
			bufferCtr = 0;
		}
	}
	return STATUS_SUCCESS;
}


NTSTATUS ReadProtectedEntitiesList() {

	UNICODE_STRING regPath = RTL_CONSTANT_STRING(REG_PATH_FOR_ANTIVIRUS);
	NTSTATUS status;

	status = WriteDataToUnicodeString(&g_hardcodedStrForFS, PROTECTED_FILES_REG_PATH);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	DEBUG_PRINT("Reg key name: %wZ", g_hardcodedStrForFS);

	status = WriteDataToUnicodeString(&g_hardcodedStrForRegistry, PROTECTED_REG_KEYS_REG_PATH);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	DEBUG_PRINT("Reg key for Registry: %wZ", g_hardcodedStrForRegistry);

	g_FS_Entities = CreateBuffer(sizeof(protectedEntity));
	if (g_FS_Entities == NULL) {
		return -1;
	}

	g_Registry_Entities = CreateBuffer(sizeof(protectedEntity));
	if (g_Registry_Entities == NULL) {
		return -1;
	}


	RTL_QUERY_REGISTRY_TABLE paramTable[] = {
		//RtlQueryRegistry routine	       Flag 			  Name  Context Default type
		{ RtlQueryRegistryRoutine, RTL_QUERY_REGISTRY_REQUIRED, NULL, NULL, REG_NONE, NULL, 0},
		{ NULL,							0,					  NULL, NULL,    0,     NULL, 0}
	};

	RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,
							regPath.Buffer,
							&paramTable[0],
							NULL,
							NULL);


	return STATUS_SUCCESS;
}