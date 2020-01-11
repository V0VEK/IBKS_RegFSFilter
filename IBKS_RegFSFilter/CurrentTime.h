#pragma once
#include <wdm.h>
#include "Common.h"

#define TIMESTAMP_LENGTH 25


NTSTATUS GetCurrentTime(PWCHAR timeStr) {
    NTSTATUS status;

    LARGE_INTEGER   SysTime;
    LARGE_INTEGER   LocalTime;
    TIME_FIELDS     TimeFields;

    KeQuerySystemTime(&SysTime);
    ExSystemTimeToLocalTime(&SysTime, &LocalTime);
    RtlTimeToTimeFields(&LocalTime, &TimeFields);

    status = RtlStringCchPrintfW(timeStr, TIMESTAMP_LENGTH, L"%02d-%02d-%04d %02d:%02d:%02d.%03d", TimeFields.Day, TimeFields.Month, TimeFields.Year, TimeFields.Hour, TimeFields.Minute, TimeFields.Second, TimeFields.Milliseconds);
    if (!NT_SUCCESS(status)) {
        DbgPrint("RtlStringCchPrintfW ERROR!");
        return status;
    }

    return STATUS_SUCCESS;
}