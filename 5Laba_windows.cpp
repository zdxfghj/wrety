#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
using namespace std;
typedef HANDLE(_cdecl* LPFNDLLinit)(char *, char *); //ñîãëàøåíèå î âûçîâàõ ïî óìîë÷àíèþ äëÿ ïðîãðàìì íà C è C ++



int main()
{
	char directory[MAX_PATH];
	char outputFileName[18] = "Vihodnoi_fail.txt";
	GetCurrentDirectory(MAX_PATH, directory);
	strcat(directory, "\\"); 
	HINSTANCE hLib = LoadLibrary("Lab5Dll.dll");		//çàãðóæàåì dll    ÿâëÿåòñÿ äåñêðèïòîðîì ýêçåìïëÿðà îêíà (ýòî íåêèé êîä îêîííîé ïðîöåäóðû, èäåíòèôèêàòîð, ïî êîòîðîé ÎÑ áóäåò îòëè÷àòü å¸ îò îñòàëüíûõ îêîí).

	LPFNDLLinit init = (LPFNDLLinit)GetProcAddress(hLib, TEXT("init")); //Ïîëó÷èòü àäðåñ èëè ôóíêöèè èç áèáëèîòåêè
	HANDLE WriteOffEvent = init(directory, outputFileName);

	//íà÷àëüíûì àäðåñîì äëÿ ïîòîêà
	LPTHREAD_START_ROUTINE  reader = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadReader"));//Ïîëó÷èòü àäðåñ ïåðåìåííîé èëè ôóíêöèè èç áèáëèîòåêè 
	LPTHREAD_START_ROUTINE  writer = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadWriter"));//Óêàçûâàåò íà ôóíêöèþ, êîòîðàÿ óâåäîìëÿåò óçåë î òîì, ÷òî ïîòîê íà÷àë âûïîëíÿòüñÿ.

	//ñîçäà¸ì ïîòîêè ÷òåíèÿ è çàïèñè
	HANDLE threadReader = CreateThread(NULL, 0, reader, NULL, 0, 0);
	HANDLE threadWriter = CreateThread(NULL, 0, writer, NULL, 0, 0);

	
	WaitForSingleObject(threadReader, INFINITE);    //Cèíõðîíèçàöèÿ ïîòîêîâ
	WaitForSingleObject(WriteOffEvent, INFINITE);
	CloseHandle(threadReader);						//î÷èùàåì äåñêðèïòîðû ïîòîêîâ
	CloseHandle(threadWriter);
	FreeLibrary(hLib);
	cout << "Funkciya proshla uspeshno!\n";
								//óáèðàåì èç ïàìÿòè dll
}
