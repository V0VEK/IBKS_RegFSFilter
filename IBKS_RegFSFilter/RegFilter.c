// Unreferenced formal parameter
#pragma warning(disable:4100)

#include <fltKernel.h>

#include "RegFilter.h"

NTSTATUS RfRegistryCallback(__in PVOID CallbackContext, __in PVOID Argument1, __in PVOID Argument2);
LARGE_INTEGER g_CmCookie = { 0 };


NTSTATUS RegFilterStart(PDRIVER_OBJECT p_driver_object) {
    //	Register our callback with the system
    UNICODE_STRING AltitudeString = RTL_CONSTANT_STRING(L"360000");
    NTSTATUS status = CmRegisterCallbackEx(RfRegistryCallback, &AltitudeString, p_driver_object, NULL, &g_CmCookie, NULL);
    if (!NT_SUCCESS(status))
    {
        return status;
        //	Failed to register - probably shouldn't succeed the driver intialization since this is critical
    }

	DEBUG_PRINT("Registry filter start successful!");
	return STATUS_SUCCESS;
}

NTSTATUS RegFilterStop() {
    NTSTATUS status = CmUnRegisterCallback(g_CmCookie);
    if (!NT_SUCCESS(status))
    {
        return status;
        //	Failed to unregister - try to handle gracefully
    }

	DEBUG_PRINT("Registry filter stop successful!");
	return STATUS_SUCCESS;
}



NTSTATUS RfRegistryCallback(__in PVOID CallbackContext, __in PVOID Argument1, __in PVOID Argument2)
{
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument2);

    REG_NOTIFY_CLASS Operation = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;
    switch (Operation)
    {
    case RegNtPreCreateKeyEx:
        break;
    case RegNtPreOpenKeyEx:
        break;
    case RegNtKeyHandleClose:
        break;
    }

    return STATUS_SUCCESS;
}
