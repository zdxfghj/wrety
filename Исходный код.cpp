#include <iostream>
#include <stack>
#include <string>
#include <conio.h>
#include <ctype.h>
#include <windows.h>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <chrono>
#include <ctime>  
#include <sys/types.h>
#include <thread>
#include <list>
#include <comdef.h>

CRITICAL_SECTION s;
std::list<CRITICAL_SECTION> sections;
HANDLE pauseEvent;
HANDLE ready;
HANDLE startWritting;
std::stack<HANDLE> threads;
int sole = 0;
bool isPause = false;

char* getRandomString(const int len)
{
	std::time_t result = std::time(nullptr);
	std::localtime(&result);

	srand(result);
	char* s = (char*)malloc(len * sizeof(char));
	static const char alphanum[] =
		"0123456789-+="
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < (len - 1); ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[(len - 1)] = alphanum[sole];

	if (sole >= len)
		sole = 0;

	sole++;
	s[len] = 0;

	return s;
}

std::wstring stringToWString(std::string string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(string);

	return wide;
}

DWORD WINAPI Potok(PVOID pvParam)
{
	char* str = getRandomString(10);
	CRITICAL_SECTION section = *(CRITICAL_SECTION*)pvParam;

	while (true)
	{
		WaitForSingleObject(startWritting, INFINITE);

		for (int i = 0; i < strlen(str); i++)
			printf_s("%c", str[i]);

		printf_s("\n");

		SetEvent(ready);
	}
}

void Writting()
{
	while (true)
	{
		if (!threads.empty())
		{
			DWORD pauseResult = WaitForSingleObject(
				pauseEvent,
				INFINITE);

			if (pauseResult == WAIT_OBJECT_0)
				SetEvent(pauseEvent);

			for (CRITICAL_SECTION section : sections)
			{
				EnterCriticalSection(&section);
				SetEvent(startWritting);
				WaitForSingleObject(ready, INFINITE);
			}

			Sleep(1000);

			printf_s("\n");
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	printf_s("Control\n");
	printf_s("\t+ � Add new process.\n");
	printf_s("\t- � Delete last process.\n");
	printf_s("\tQ � Quit.\n");
	printf_s("\tP � Turn on/off pause.\n");
	printf_s("\n");

	int ch;
	InitializeCriticalSection(&s);
	std::thread t1(Writting);

	t1.detach();

	pauseEvent = CreateEvent(
		NULL,
		TRUE,
		TRUE,
		L"PAUSE"
	);

	if (pauseEvent == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 0;
	}

	ready = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		L"READY"
	);

	if (ready == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 0;
	}

	startWritting = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		L"START_WRITTING"
	);

	if (startWritting == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 0;
	}

	while (true)
	{
		ch = _getch();
		ch = toupper(ch);

		switch (ch)
		{
		case '+':
		{
			if (isPause)
				break;
	
			CRITICAL_SECTION section;
			InitializeCriticalSection(&section);
			sections.push_back(section);

			HANDLE thread = CreateThread(NULL, NULL, Potok, (PVOID*)&section, NULL, NULL);

			threads.push(thread);

			break;
		}
		case '-':
		{
			if (isPause)
				break;

			if (!threads.empty())
			{
				TerminateThread(threads.top(), 0);
				threads.pop();
				DeleteCriticalSection(&sections.back());
				sections.pop_back();
			}

			break;
		}
		case 'Q':
		{
			while (!threads.empty())
			{
				TerminateThread(threads.top(), 0);
				threads.pop();
				DeleteCriticalSection(&sections.back());
				sections.pop_back();
			}

			return 0;
		}
		case 'P':
		{
			if (!isPause)
				ResetEvent(pauseEvent);
			else
				SetEvent(pauseEvent);

			isPause = !isPause;

			break;
		}
		}
	}

	return 0;
}
