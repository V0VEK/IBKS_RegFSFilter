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