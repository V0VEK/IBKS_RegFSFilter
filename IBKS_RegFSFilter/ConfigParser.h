#pragma once
#include <fltKernel.h>
#include"Logger.h"

#define PROTECTED_REGKEYS_REG_PATH ""
#define PROTECTED_FILES_REG_PATH ""


NTSTATUS ReadProtectedEntitiesList(PUNICODE_STRING regPath);