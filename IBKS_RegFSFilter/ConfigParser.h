#pragma once
#include <fltKernel.h>
#include <ntstrsafe.h>

#include "Logger.h"
#include "Common.h"

#define REG_PATH_FOR_ANTIVIRUS (L"\\REGISTRY\\MACHINE\\SOFTWARE\\IBKS_AV")
#define PROTECTED_FILES_REG_PATH (L"ProtectedFiles")
#define PROTECTED_REG_KEYS_REG_PATH (L"ProtectedRegKeys")
#define MAX_BUFF_SIZE 1000

typedef struct protectedEntity {
	UNICODE_STRING path;
	struct protectedEntity* Next;
} protectedEntity;

protectedEntity* g_FS_Entities;
protectedEntity* g_Registry_Entities;

UNICODE_STRING g_hardcodedStrForFS;
UNICODE_STRING g_hardcodedStrForRegistry;


NTSTATUS ReadProtectedEntitiesList();

protectedEntity* FillListOfProtectedEntities(protectedEntity* list, PUNICODE_STRING data);

VOID DeleteProtectedEntitiesList();

VOID DeleteList(protectedEntity* list);