#include <fltKernel.h>

#include "FsFilter.h"


PFLT_FILTER g_FSFilterHandle = NULL;

// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_pre_operation_callback
// Обработчик событий, который будет регистрироваться фильтром
FLT_POSTOP_CALLBACK_STATUS FsFilterEventHandler
(
	// Данные о текущей операции ввода/вывода, здесь
	// фильтр отражает изменения в поведении системы
	PFLT_CALLBACK_DATA       data,

	// Объекты, связанные с операцией ввода/вывода
	// (ссылка на файл, раздел и т.д.
	PCFLT_RELATED_OBJECTS    flt_objects,
	PVOID                    completion_context,
	FLT_POST_OPERATION_FLAGS flags
);


// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_operation_registration
const FLT_OPERATION_REGISTRATION fsFilterCallbacks[] = {
	{IRP_MJ_CREATE, 0, NULL, FsFilterEventHandler},
	{IRP_MJ_OPERATION_END}
};

// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
const FLT_REGISTRATION fsFilterRegistrationTable = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	fsFilterCallbacks,
	FsFilterStop,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


NTSTATUS FsFilterStart(PDRIVER_OBJECT p_driver_object) {

	NTSTATUS status = FltRegisterFilter(p_driver_object, &fsFilterRegistrationTable, &g_FSFilterHandle);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	/*status = FltStartFiltering(g_FSFilterHandle);
	if (!NT_SUCCESS(status)) {
		FltUnregisterFilter(g_FSFilterHandle);
		return status;
	}*/

	DEBUG_PRINT("Filesystem filter start successfull!");
	DEBUG_PRINT("Dumping files that need to be protected: ");
	DumpAllList(g_FS_Entities);

	return STATUS_SUCCESS;
}


NTSTATUS FsFilterStop() {
	DEBUG_PRINT("Before stop");
	FltUnregisterFilter(g_FSFilterHandle);
	DEBUG_PRINT("Filesystem filter stop successfull!");
	return STATUS_SUCCESS;
}


FLT_POSTOP_CALLBACK_STATUS FsFilterEventHandler(PFLT_CALLBACK_DATA       data,
	PCFLT_RELATED_OBJECTS    flt_objects,
	PVOID                    completion_context,
	FLT_POST_OPERATION_FLAGS flags) 
{


	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}