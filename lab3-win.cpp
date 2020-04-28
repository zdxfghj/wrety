#include <iostream>
#include <Windows.h>

#define COUNT 1
#define MAX_BUFFER_LENGTH 255

using namespace std;

HANDLE create_process(char *path) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(path, (LPSTR)"process chat", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return pi.hProcess;
}

void close_process(HANDLE process) {
	TerminateProcess(process, 0);
	CloseHandle(process);
}

void main_process(char *path) {
	HANDLE shared_memory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_BUFFER_LENGTH, "Global\\CurrentProcessSize");
	if (shared_memory == NULL) {
		cout << "Error with shared memory" << endl;
	}

	HANDLE semaphore = CreateSemaphore(NULL, 0, COUNT, "Global\\posted");
	if (semaphore == NULL) {
		cout << "Error with semaphore" << endl;
	}

	HANDLE process = create_process(path);
	cout << "Send message to client:" << endl;

	while (true) {
		cout << "> ";
		string message;
		cin >> message;

		PVOID buffer = MapViewOfFile(shared_memory, FILE_MAP_ALL_ACCESS, 0, 0, MAX_BUFFER_LENGTH);
		if (buffer == NULL) {
			cout << "Error with buffer" << endl;
		}

		if (message == "quit") {
			cout << "Good Bye." << endl;
			close_process(process);
			getchar();
			exit(0);
		}

		memset(buffer, NULL, MAX_BUFFER_LENGTH);
		CopyMemory(buffer, (PVOID)message.c_str(), message.size());

		if (!ReleaseSemaphore(semaphore, 1, NULL)) {
			cout << "Error" << endl;
		}
		WaitForSingleObject(semaphore, INFINITE);
	}
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		while (true) {
			system("cls");
			cout << "Waiting for server message..." << endl;

			HANDLE semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "Global\\posted");
			if (semaphore == NULL) {
				cout << "Error with semaphore" << endl;
			}

			WaitForSingleObject(semaphore, INFINITE);
			system("cls");

			HANDLE shared_memory = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Global\\CurrentProcessSize");
			if (shared_memory == NULL) {
				cout << "Error with shared memory" << endl;
			}

			PVOID buffer = MapViewOfFile(shared_memory, FILE_MAP_ALL_ACCESS, 0, 0, MAX_BUFFER_LENGTH);
			if (buffer == NULL) {
				cout << "Error with buffer" << endl;
			}

			string message = (char*)buffer;
			cout << "Server message: " << message << endl;

			if (ReleaseSemaphore(semaphore, 1, NULL) == NULL) {
				cout << "Error with releasing" << endl;
			}

			getchar();
		}
	}
	else main_process(argv[0]);

	return 0;
}