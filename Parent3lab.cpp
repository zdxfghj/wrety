#include <iostream>
#include <Windows.h>

#include <stdio.h>

using namespace std;

int main()
{

	STARTUPINFO si = { sizeof(si) };
	ZeroMemory(&si, sizeof(si));
	si.lpTitle = (LPSTR)L"Children!)";

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));


	if (!CreateProcess(NULL, (LPSTR)"Children.exe", NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		cout << "Error "+ GetLastError() << endl;
		system("pause");
		return 0;
	}
	printf("PID:%d",pi.dwProcessId);

	

	LPSTR cmdline = (LPSTR)"\\\\.\\pipe\\Mypipi";
	HANDLE chanel;

	chanel=CreateNamedPipe(cmdline, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE,
		PIPE_UNLIMITED_INSTANCES,512,512,5000, NULL);
	if (chanel == INVALID_HANDLE_VALUE)
	{
		cout << "Error " + GetLastError() << endl;

		system("pause");
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return 0;
	}

	cout << "Waiting for client..." << endl;

	BOOL isconnect = false;
		isconnect = ConnectNamedPipe(chanel, NULL);
	if (isconnect == false)
	{
		cout << GetLastError() << endl;
		CloseHandle(chanel);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		system("pause");
		return 0;
	}

	cout << "Client is connect.))" << endl;
	
	HANDLE temp = CreateEventA(NULL, FALSE, FALSE, (LPCSTR)"ONESECOND");
	
	char command[256] = "\0";
	DWORD cWrite;
	while (true) {
		fgets(command,512,stdin);
		
		if(WriteFile(chanel, command, strlen(command) + 1, &cWrite, NULL)) {
			SetEvent(temp);
			if (WaitForSingleObject(temp, 2000) == WAIT_OBJECT_0) {
				printf("Hello for client\n");
				ResetEvent(temp);
			}
			if (!strcmp(command, "EXIT\n")) {
				break;
			}
			
		}
		else
		{
			cout << GetLastError() << endl;
			system("pause");
			CloseHandle(chanel);
	        CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return 0;
		}
		
	}
	CloseHandle(chanel);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	system("pause");
	return 0;
}
