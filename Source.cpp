#include <Windows.h>
#include <stdio.h>

int main() {
	STARTUPINFO si = { sizeof(si) };
	ZeroMemory(&si, sizeof(si));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	
	HANDLE createThread = CreateEvent(NULL, FALSE, FALSE, "createEvent");
	HANDLE deleteThread = CreateEvent(NULL, FALSE, FALSE, "deleteEvent");
	HANDLE exitProcess  = CreateEvent(NULL, FALSE, FALSE, "exitProcess");

	if (CreateProcess(NULL, (LPSTR)"Child.exe", NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) == NULL) {
		printf("Error open Process");
		return 0;
	}

	printf("Child process create:%lu",pi.dwProcessId);

	wchar_t text;
	while (true) {
		text = getwchar();
		switch (text)
		{
		case L'+':{
			SetEvent(createThread);
			break; }
		case L'-':{ 
			SetEvent(deleteThread);
			break; }
		case L'exit': {
			SetEvent(exitProcess);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			CloseHandle(createThread);
			CloseHandle(deleteThread);
			CloseHandle(exitProcess);
			return 0; }
		}


	}

}