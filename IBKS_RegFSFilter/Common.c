#include "Common.h"



NTSTATUS WriteDataToUnicodeString(PUNICODE_STRING dest, PVOID source) {
	size_t length;
	NTSTATUS status;

	status = RtlStringCchLengthW((STRSAFE_PCNZWCH)source, NTSTRSAFE_MAX_CCH, &length);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	dest->Buffer = (PWCH)source;
	dest->Length = (length + 1) * sizeof(WCHAR);
	dest->MaximumLength = dest->Length;

	return STATUS_SUCCESS;
}

PVOID CreateBuffer(size_t size) {
	PVOID buffer = ExAllocatePool(NonPagedPool, size);
	if (buffer == NULL) {
		return NULL;	// Maybe need to fix
	}
	RtlZeroMemory(buffer, size);

	return buffer;
}