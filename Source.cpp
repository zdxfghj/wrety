#include<stdio.h>

#include<Windows.h>
#include<tchar.h>


int main()
{
	HANDLE hNamedPipe;
	LPSTR  ipNamePipe = (LPSTR)"\\\\.\\pipe\\Pipe228";

	hNamedPipe = CreateFile(ipNamePipe, GENERIC_WRITE | GENERIC_READ, 0, NULL,
		OPEN_EXISTING, 0, NULL);
	
	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		printf("CreateFile error #%lu\n", GetLastError());

		system("pause");
		return 0;
	}

	printf("Connected.\n");
	char commandBuf[256] = "\0";
	DWORD cWritten;
	DWORD cREAD;

	while (true) {
		//if (!WriteFile(hNamedPipe, commandBuf, strlen(commandBuf) + 1, &cWritten, NULL))
		//	break;

		if (ReadFile(hNamedPipe, commandBuf, 512, &cREAD, NULL)) {
			printf("%s", commandBuf);

			if (!strcmp(commandBuf, "exit "))
				break;
		}
		else {
			printf("ReadFile error #%lu\n", GetLastError());

			system("pause");
			CloseHandle(hNamedPipe);
			return 0;
		}
	}

	CloseHandle(hNamedPipe);
	return 0;
}