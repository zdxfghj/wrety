#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
using namespace std;
typedef HANDLE(_cdecl* LPFNDLLinit)(char *, char *); //���������� � ������� �� ��������� ��� �������� �� C � C ++



int main()
{
	char directory[MAX_PATH];
	char outputFileName[18] = "Vihodnoi_fail.txt";
	GetCurrentDirectory(MAX_PATH, directory);
	strcat(directory, "\\"); 
	HINSTANCE hLib = LoadLibrary("Lab5Dll.dll");		//��������� dll    �������� ������������ ���������� ���� (��� ����� ��� ������� ���������, �������������, �� ������� �� ����� �������� � �� ��������� ����).

	LPFNDLLinit init = (LPFNDLLinit)GetProcAddress(hLib, TEXT("init")); //�������� ����� ��� ������� �� ����������
	HANDLE WriteOffEvent = init(directory, outputFileName);

	//��������� ������� ��� ������
	LPTHREAD_START_ROUTINE  reader = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadReader"));//�������� ����� ���������� ��� ������� �� ���������� 
	LPTHREAD_START_ROUTINE  writer = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadWriter"));//��������� �� �������, ������� ���������� ���� � ���, ��� ����� ����� �����������.

	//������ ������ ������ � ������
	HANDLE threadReader = CreateThread(NULL, 0, reader, NULL, 0, 0);
	HANDLE threadWriter = CreateThread(NULL, 0, writer, NULL, 0, 0);

	
	WaitForSingleObject(threadReader, INFINITE);    //C������������ �������
	WaitForSingleObject(WriteOffEvent, INFINITE);
	CloseHandle(threadReader);						//������� ����������� �������
	CloseHandle(threadWriter);
	FreeLibrary(hLib);
	cout << "Funkciya proshla uspeshno!\n";
								//������� �� ������ dll
}