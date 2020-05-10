#include <stdio.h>
#include <Windows.h>
#include <iostream>

using namespace std;

int main()
{
	LPSTR cmdline = (LPSTR)"\\\\.\\pipe\\Mypipi";
	HANDLE namepipe;
	namepipe = CreateFile(cmdline, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);


	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, (LPCSTR)"ONESECOND");

	if (namepipe == INVALID_HANDLE_VALUE)
	{
		cout << "error: " + GetLastError() << endl;
		system("pause");
		return 0;
	}

	cout << "Connected:)"<<endl;
	char command[256] = "\0";
	DWORD cRead;

	while (true)
	{

		if (WaitForSingleObject(hEvent, 3000) == WAIT_OBJECT_0) {
			printf("It's ok\n");
			ResetEvent(hEvent);
			
			if (ReadFile(namepipe, command, 512, &cRead, NULL))
			{
				SetEvent(hEvent);
				cout << command;
				if (!strcmp(command, "EXIT\n")) {
					break;
				}
			}
			else {
				cout << GetLastError() << endl;
				system("pause");
				CloseHandle(namepipe);
				return 0;
			}
		}
	}
	CloseHandle(namepipe);
	system("pause");

	return 0;
}
