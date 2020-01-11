#include <windows.h>
#include <fltuser.h>
#include <iostream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "fltlib.lib")

#define COM_PORT_NAME_FS L"\\IBKS_FS_Filter_COM_Port"
#define COM_PORT_NAME_REG L"\\IBKS_REG_Filter_COM_Port"
#define COM_BUFFER_SIZE 1000


HANDLE g_hPort_FS;
HANDLE g_hPort_REG;


typedef struct DATA_RECEIVE {
	FILTER_MESSAGE_HEADER msgHeader;
	WCHAR msg[COM_BUFFER_SIZE] = { 0 };
} DATA_RECEIVE;


// https://stackoverflow.com/questions/127124/resolve-windows-device-path-to-drive-letter
int DeviceNameToVolumePathName(WCHAR* filepath) {
    WCHAR fileDevName[MAX_PATH];
    WCHAR devName[MAX_PATH];
    WCHAR fileName[MAX_PATH];
    HANDLE FindHandle = INVALID_HANDLE_VALUE;
    WCHAR  VolumeName[MAX_PATH];
    DWORD  Error = ERROR_SUCCESS;
    size_t Index = 0;
    DWORD  CharCount = MAX_PATH + 1;

    int index = 0;
    // \Device\HarddiskVolume1\windows,locate \windows.
    for (int i = 0; i < lstrlenW(filepath); i++) {
        if (!memcmp(&filepath[i], L"\\", 2)) {
            index++;
            if (index == 3) {
                index = i;
                break;
            }
        }
    }
    filepath[index] = L'\0';

    memcpy(fileDevName, filepath, ((unsigned long long)index + (unsigned long long)1) * sizeof(WCHAR));

    FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

    if (FindHandle == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
        return FALSE;
    }
    for (;;)
    {
        //  Skip the \\?\ prefix and remove the trailing backslash.
        Index = wcslen(VolumeName) - 1;

        if (VolumeName[0] != L'\\' ||
            VolumeName[1] != L'\\' ||
            VolumeName[2] != L'?' ||
            VolumeName[3] != L'\\' ||
            VolumeName[Index] != L'\\')
        {
            Error = ERROR_BAD_PATHNAME;
            wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
            break;
        }
        VolumeName[Index] = L'\0';
        CharCount = QueryDosDeviceW(&VolumeName[4], devName, 100);
        if (CharCount == 0)
        {
            Error = GetLastError();
            wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
            break;
        }
        if (!lstrcmpW(devName, filepath)) {
            VolumeName[Index] = L'\\';
            Error = GetVolumePathNamesForVolumeNameW(VolumeName, fileName, CharCount, &CharCount);
            if (!Error) {
                Error = GetLastError();
                wprintf(L"GetVolumePathNamesForVolumeNameW failed with error code %d\n", Error);
                break;
            }

            // concat drive letter to path
            lstrcatW(fileName, &filepath[index + 1]);
            lstrcpyW(filepath, fileName);

            Error = ERROR_SUCCESS;
            break;
        }

        Error = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

        if (!Error)
        {
            Error = GetLastError();

            if (Error != ERROR_NO_MORE_FILES)
            {
                wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
                break;
            }

            //
            //  Finished iterating
            //  through all the volumes.
            Error = ERROR_BAD_PATHNAME;
            break;
        }
    }

    FindVolumeClose(FindHandle);
    if (Error != ERROR_SUCCESS)
        return FALSE;
    return TRUE;

}


int ConnectToCOM() {

	HRESULT res;

	while (1) {
		res = FilterConnectCommunicationPort(COM_PORT_NAME_FS, 0, NULL, 0, NULL, &g_hPort_FS);
		if (res == S_OK) {
			break;
		}
	}

	std::cout << "Connected to FS_COM_PORT!" << std::endl;

	/*while (1) {
		res = FilterConnectCommunicationPort(COM_PORT_NAME_REG, 0, NULL, 0, NULL, &g_hPort_FS);
		if (res == S_OK) {
			break;
		}
	}*/


	std::cout << "Connected to REG_COM_PORT!" << std::endl;

	return 0;
}


DWORD WINAPI ReadMessagesFromFSCOM() {
	HRESULT res;
	DATA_RECEIVE data;
	memset(&data, NULL, sizeof(data));
	std::cout << "Reading message; sizeof(data) = " << sizeof(data) << std::endl;

	while (1) {
		res = FilterGetMessage(g_hPort_FS, &data.msgHeader, sizeof(data), NULL);
		if (res != S_OK) {
			return -1;
		}
		std::wcout << L"LOOOL REG: " << data.msg << std::endl;
		memset(&data, NULL, sizeof(data));
	}

	ExitThread(0);
}




DWORD WINAPI ReadMessagesFromREGCOM() {
	HRESULT res;
	DATA_RECEIVE* data = (DATA_RECEIVE*)malloc(sizeof(DATA_RECEIVE));
	if (data == NULL) {
		return -1;
	}
	memset(data, NULL, sizeof(DATA_RECEIVE));

	while (1) {
		res = FilterGetMessage(g_hPort_REG, &data->msgHeader, COM_BUFFER_SIZE * sizeof(WCHAR), NULL);
		if (res != S_OK) {
			free(data);
			return -1;
		}
		std::wcout << L"LOOOL REG: " << data->msg << std::endl;
		memset(data, NULL, sizeof(DATA_RECEIVE));
	}

	ExitThread(0);
}



int main() {


	g_hPort_FS = NULL;
	g_hPort_REG = NULL;

	if (ConnectToCOM() != 0) {
		return -1;
	}

	HANDLE FS_COM_ReaderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadMessagesFromFSCOM, NULL, 0, 0);
	if (FS_COM_ReaderThread == NULL) {
		return -1;
	}

	HANDLE REG_COM_ReaderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadMessagesFromREGCOM, NULL, 0, 0);
	if (REG_COM_ReaderThread == NULL) {
		return -1;
	}


	WaitForSingleObject(FS_COM_ReaderThread, INFINITE);
	WaitForSingleObject(REG_COM_ReaderThread, INFINITE);

	CloseHandle(FS_COM_ReaderThread);
	CloseHandle(REG_COM_ReaderThread);


	return 0;
}