// Unreferenced formal parameter
#pragma warning( disable : 4100)

#include "ConfigParser.h"


RTL_QUERY_REGISTRY_ROUTINE RtlQueryRegistryRoutine;

// This function is called for each registry key that was requested
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

		g_FS_Entities = NULL;
		// Read buffer of registry key string value and fill list of protected files to use this list in real time to protect
		g_FS_Entities = FillListOfProtectedEntities(g_FS_Entities, &valueData);
		if (status != STATUS_SUCCESS) {
			return status;
		}
	}

	if (RtlEqualUnicodeString(&valueName, &g_hardcodedStrForRegistry, TRUE)) {
		DEBUG_PRINT("Fill list for registry");

		g_Registry_Entities = NULL;
		// Read buffer of registry key string value and fill list of protected registry keys to use this list in real time to protect
		g_Registry_Entities = FillListOfProtectedEntities(g_Registry_Entities, &valueData);
		if (status != STATUS_SUCCESS) {
			return status;
		}
	}

	return STATUS_SUCCESS;
}


protectedEntity* AddPathToList(protectedEntity* list, UNICODE_STRING str) {
	protectedEntity* newElem = CreateBuffer(sizeof(protectedEntity));
	if (newElem == NULL) {
		return NULL;
	}
	newElem->path = str;
	newElem->Next = list;
	return newElem;
}


protectedEntity* FillListOfProtectedEntities(protectedEntity* list, PUNICODE_STRING data) {
	if (data->Buffer[0] == L'\0') {
		return NULL;
	}
	
	int i = 0;
	int bufferCtr = 0;
	protectedEntity* firstElem = list;

	WCHAR* buffer;
	// Creates buffer of proper size and zeroes memory
	buffer = CreateBuffer(MAX_BUFF_SIZE * sizeof(WCHAR));
	if (buffer == NULL) {
		return NULL;
	}

	while (data->Buffer[i] != 0) {
		buffer[bufferCtr] = data->Buffer[i];
		bufferCtr++;
		if (data->Buffer[i] == L';') {
			buffer[bufferCtr - 1] = L'\0';

			UNICODE_STRING str;
			RtlInitUnicodeStringEx(&str, buffer);

			firstElem = AddPathToList(firstElem, str);
			DEBUG_PRINT("Added: %wZ", firstElem->path);
			if (firstElem == NULL) {
				return NULL;
			}

			buffer = CreateBuffer(MAX_BUFF_SIZE * sizeof(WCHAR));
			bufferCtr = 0;
		}
		i++;
	}
	return firstElem;
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

	// This is neccessary to read registry key from kernel mode
	RTL_QUERY_REGISTRY_TABLE paramTable[] = {
		//RtlQueryRegistry routine	       Flag 			  Name  Context Default type
		{ RtlQueryRegistryRoutine, RTL_QUERY_REGISTRY_REQUIRED, NULL, NULL, REG_NONE, NULL, 0},
		{ NULL,							0,					  NULL, NULL,    0,     NULL, 0}
	};

	// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlqueryregistryvalues
	RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE,
							regPath.Buffer,
							&paramTable[0],
							NULL,
							NULL);


	return STATUS_SUCCESS;
}


VOID DeleteList(protectedEntity* list) {
	protectedEntity* node = list;
	protectedEntity* tmp;

	while (node->Next != NULL) {
		tmp = node;
		node = node->Next;

		ExFreePoolWithTag(tmp->path.Buffer, '1gaT');
		ExFreePoolWithTag(tmp, '1gaT');

	}

	ExFreePoolWithTag(node->path.Buffer, '1gaT');
	ExFreePoolWithTag(node, '1gaT');
}


VOID DeleteProtectedEntitiesList() {
	DeleteList(g_FS_Entities);
	DeleteList(g_Registry_Entities);
}


VOID DumpAllList(protectedEntity* list) {
	protectedEntity* node = list;
	int i = 0;
	while (1) {
		DEBUG_PRINT("%d. %wZ", i, node->path);
		i++;
		node = node->Next;
		if (node == NULL)
			break;
	}
}