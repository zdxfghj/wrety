#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <vector>

int temp_x = 0;
int countthread = 0;
DWORD readID[120];
std::vector<HANDLE> vecthread;

CRITICAL_SECTION crit;

HANDLE deletThread = CreateEvent(NULL, FALSE, FALSE, "deleteEvent");

void sum() {
	while (true) {
		EnterCriticalSection(&crit);
		if (WaitForSingleObject(deletThread, 0) == WAIT_OBJECT_0) {
			if (GetCurrentThreadId() == readID[countthread-1]) {
				CloseHandle(vecthread.back());
				vecthread.pop_back();
				printf("Close last thred.");
				readID[countthread - 1] = -1;
				countthread--;
				LeaveCriticalSection(&crit);
				return;
			}
			SetEvent(deletThread);
		}
		printf("Thread: %d  -->%d<--\n",GetCurrentThreadId(),temp_x);
		temp_x++;
		LeaveCriticalSection(&crit);
	}
}


int main() {
	HANDLE createThread = CreateEvent(NULL, FALSE, FALSE, "createEvent");
	HANDLE exitProcess  = CreateEvent(NULL, FALSE, FALSE, "exitProcess");

	InitializeCriticalSection(&crit);

	while (true) {
		if (WaitForSingleObject(createThread, 0) == WAIT_OBJECT_0) {
			vecthread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sum,
				NULL, 0, &readID[countthread]));
			countthread++;
		}
		if (WaitForSingleObject(exitProcess, 0) == WAIT_OBJECT_0) {
			for (int i = 0; i < vecthread.size(); i++) {
				CloseHandle(vecthread[i]);
			}
			CloseHandle(createThread);
			CloseHandle(exitProcess);
			CloseHandle(deletThread);
			DeleteCriticalSection(&crit);
			system("pause");
			return 0;
		}
	}
}