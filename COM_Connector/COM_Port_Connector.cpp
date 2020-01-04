#include <windows.h>
#include <fltuser.h>
#include <iostream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "fltlib.lib")

#define COM_PORT_NAME L"IBKS_RegFS_Filter_COM_Port"
#define COM_BUFFER_SIZE 1000


HANDLE g_hPort;

int ConnectToCOM() {

	HRESULT res = FilterConnectCommunicationPort(COM_PORT_NAME, 0, NULL, 0, NULL, &g_hPort);
	if (res != S_OK) {
		return -1;
	}

	return 0;
}


int ReadMessagesFromCOM() {
	HRESULT res;
	unsigned char* buffer = (unsigned char*)malloc(COM_BUFFER_SIZE);
	memset(buffer, NULL, COM_BUFFER_SIZE);

	while (1) {
		res = FilterGetMessage(g_hPort, (PFILTER_MESSAGE_HEADER)buffer, COM_BUFFER_SIZE, NULL);
		if (res != S_OK) {
			free(buffer);
			return -1;
		}
		std::cout << "LOOOL: " << buffer << std::endl;
		memset(buffer, NULL, COM_BUFFER_SIZE);
	}


	return 0;
}


int main() {

	g_hPort = NULL;

	if (ConnectToCOM() != 0) {
		return -1;
	}

	return 0;
}