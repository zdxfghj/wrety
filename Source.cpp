#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
using namespace std;
typedef HANDLE(_cdecl* LPFNDLLinit)(char *, char *); //соглашение о вызовах по умолчанию для программ на C и C ++



int main()
{
	char directory[MAX_PATH];
	char outputFileName[18] = "Vihodnoi_fail.txt";
	GetCurrentDirectory(MAX_PATH, directory);
	strcat(directory, "\\"); 
	HINSTANCE hLib = LoadLibrary("Lab5Dll.dll");		//загружаем dll    является дескриптором экземпляра окна (это некий код оконной процедуры, идентификатор, по которой ОС будет отличать её от остальных окон).

	LPFNDLLinit init = (LPFNDLLinit)GetProcAddress(hLib, TEXT("init")); //Получить адрес или функции из библиотеки
	HANDLE WriteOffEvent = init(directory, outputFileName);

	//начальным адресом для потока
	LPTHREAD_START_ROUTINE  reader = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadReader"));//Получить адрес переменной или функции из библиотеки 
	LPTHREAD_START_ROUTINE  writer = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadWriter"));//Указывает на функцию, которая уведомляет узел о том, что поток начал выполняться.

	//создаём потоки чтения и записи
	HANDLE threadReader = CreateThread(NULL, 0, reader, NULL, 0, 0);
	HANDLE threadWriter = CreateThread(NULL, 0, writer, NULL, 0, 0);

	
	WaitForSingleObject(threadReader, INFINITE);    //Cинхронизация потоков
	WaitForSingleObject(WriteOffEvent, INFINITE);
	CloseHandle(threadReader);						//очищаем дескрипторы потоков
	CloseHandle(threadWriter);
	FreeLibrary(hLib);
	cout << "Funkciya proshla uspeshno!\n";
								//убираем из памяти dll
}