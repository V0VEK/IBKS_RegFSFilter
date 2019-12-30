// Unreferenced formal parameter
#pragma warning(disable:4100)
#pragma warning(disable:6011)

#include <fltKernel.h>

#include "FsFilter.h"


PFLT_FILTER g_FSFilterHandle = NULL;

// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_pre_operation_callback
FLT_PREOP_CALLBACK_STATUS PreFileOperationCallback(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext
);


// https://docs.microsoft.com/ru-ru/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_operation_registration
const FLT_OPERATION_REGISTRATION fsFilterCallbacks[] = {
	{IRP_MJ_CREATE, 0, PreFileOperationCallback, NULL},
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

	status = FltStartFiltering(g_FSFilterHandle);
	if (!NT_SUCCESS(status)) {
		FltUnregisterFilter(g_FSFilterHandle);
		return status;
	}

	DEBUG_PRINT("Filesystem filter start successfull!");
	DEBUG_PRINT("Dumping files that need to be protected: ");
	DumpAllList(g_FS_Entities);

	return STATUS_SUCCESS;
}


NTSTATUS FsFilterStop(FLT_FILTER_UNLOAD_FLAGS Flags) {
	DEBUG_PRINT("Before stop");
	FltUnregisterFilter(g_FSFilterHandle);
	DEBUG_PRINT("Filesystem filter stop successfull!");
	return STATUS_SUCCESS;
}


// https://www.osronline.com/article.cfm%5Earticle=472.htm
typedef NTSTATUS(*QUERY_INFO_PROCESS) (
    __in HANDLE ProcessHandle,
    __in PROCESSINFOCLASS ProcessInformationClass,
    __out_bcount(ProcessInformationLength) PVOID ProcessInformation,
    __in ULONG ProcessInformationLength,
    __out_opt PULONG ReturnLength
    );

QUERY_INFO_PROCESS ZwQueryInformationProcess;

NTSTATUS GetProcessImageName(PUNICODE_STRING ProcessImageName)
{
    NTSTATUS status;
    ULONG returnedLength;
    ULONG bufferLength;
    PVOID buffer;
    PUNICODE_STRING imageName;

    PAGED_CODE(); // this eliminates the possibility of the IDLE Thread/Process

    if (NULL == ZwQueryInformationProcess) {
        UNICODE_STRING routineName;
        RtlInitUnicodeString(&routineName, L"ZwQueryInformationProcess");
        ZwQueryInformationProcess = (QUERY_INFO_PROCESS)MmGetSystemRoutineAddress(&routineName);

        if (NULL == ZwQueryInformationProcess) {
            DbgPrint("Cannot resolve ZwQueryInformationProcess\n");
        }
    }
    //
    // Step one - get the size we need
    //
    status = ZwQueryInformationProcess(NtCurrentProcess(),
        ProcessImageFileName,
        NULL, // buffer
        0, // buffer size
        &returnedLength);

    if (STATUS_INFO_LENGTH_MISMATCH != status) {
        return status;
    }

    //
    // Is the passed-in buffer going to be big enough for us? 
    // This function returns a single contguous buffer model...
    //
    bufferLength = returnedLength - sizeof(UNICODE_STRING);
    if (ProcessImageName->MaximumLength < bufferLength) {
        ProcessImageName->Length = (USHORT)bufferLength;
        return STATUS_BUFFER_OVERFLOW;
    }

    //
    // If we get here, the buffer IS going to be big enough for us, so
    // let's allocate some storage.
    //
    buffer = ExAllocatePoolWithTag(PagedPool, returnedLength, 'ipgD');

    if (NULL == buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now lets go get the data
    //
    status = ZwQueryInformationProcess(NtCurrentProcess(),
        ProcessImageFileName,
        buffer,
        returnedLength,
        &returnedLength);

    if (NT_SUCCESS(status)) {
        //
        // Ah, we got what we needed
        //
        imageName = (PUNICODE_STRING)buffer;
        RtlCopyUnicodeString(ProcessImageName, imageName);
    }

    //
    // free our buffer
    //
    ExFreePool(buffer);

    //
    // And tell the caller what happened.
    //   
    return status;
}


// https://habr.com/ru/post/176739/
FLT_PREOP_CALLBACK_STATUS PreFileOperationCallback(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext
)
{
	NTSTATUS status;
	PFILE_OBJECT FileObject;
	UNICODE_STRING processName;

	if (FLT_IS_FS_FILTER_OPERATION(Data))
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	if (FltObjects->FileObject != NULL && Data != NULL) {
		FileObject = Data->Iopb->TargetFileObject;
		if (FileObject != NULL && Data->Iopb->MajorFunction == IRP_MJ_CREATE) {
			processName.Length = 0;
			processName.MaximumLength = NTSTRSAFE_UNICODE_STRING_MAX_CCH * sizeof(WCHAR);
			processName.Buffer = CreateBuffer(processName.MaximumLength);
			if (processName.Buffer == NULL) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
			
			status = GetProcessImageName(&processName);

			if (!NT_SUCCESS(status)) {
				ExFreePoolWithTag(processName.Buffer, MY_POOL_TAG);
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
			DEBUG_PRINT("Process name: %wZ; File name: %wZ", processName, FileObject->FileName);

            ExFreePoolWithTag(processName.Buffer, MY_POOL_TAG);
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}