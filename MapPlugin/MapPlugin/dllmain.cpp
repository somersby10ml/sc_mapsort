#include "stdafx.h"

HMODULE hInst;

#ifdef _UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

class cMapInfo {
public:
	tstring MapName;
};


//cMapInfo Map;
//std::vector<cMapInfo> Maps;
//std::vector<cMapInfo>::iterator i;


HWND hListBox;
BOOL bIsShow = FALSE;

// 스타 맵정렬 구조체
typedef struct tagMapList {
	tagMapList* Prev;
	tagMapList* Next;
	char Buffer[100];
} *PMapList;


// 실제함수 선형리스트를 조회해서 정렬한다.
void __stdcall SortList(DWORD Pointer , DWORD *RetnPointer) {

	PMapList pMapFirst = (PMapList)Pointer;
	PMapList pMap;			// 순회할 맴버
	PMapList pMapFolder;	// 폴더가 있는곳
	tagMapList* Next;

	pMap = pMapFirst;

	DWORD ItemCount = 0;
	DWORD i = 0;
	BOOL isGoMap = TRUE;
	BOOL First = TRUE;

	*RetnPointer = (DWORD)pMapFirst;

	// 전체 아이템이 2개 이하면 정렬할 필요없이 나감
	if ((signed)pMap <= 0) return;
	do
	{
		ItemCount++;
		pMap = pMap->Next;
	} while ((signed)pMap > 0);
	if (ItemCount < 6) return;

	// pMapFirst = 리스트 처음 시작
	// pMapFolder = 맵 처음 시작
	pMap = pMapFirst;
	pMapFolder = pMapFirst;

	do 
	{
		// 맵파일이면
		if (pMap->Buffer[lstrlenA(pMap->Buffer) - 1] == ']')
		{
			// 맵이 끝나는 지점을 찾음 (거기서부터 정렬할꺼임)
			// 맵의 끝나는곳을 찾음
			if (isGoMap)
			{
				First = FALSE;
				pMapFolder = pMap->Next;
				pMap = pMap->Next;
				// 폴더만 반복하다 뒤지면 루프를 나감
				if ((signed)pMapFolder <= 0) break;
				goto l_result;
			}
			

			// 폴더를 찾으면 링크를 끊는다.
			if ((signed)pMap->Next > 0)
			{
				pMap->Prev->Next = pMap->Next;
				pMap->Next->Prev = pMap->Prev;
			}
			else // 다음 맴버가 없으면
			{
				pMap->Prev->Next = pMap->Next;
			}

			// pMapFolder 전 맴버에 삽입
			Next = pMap->Next;
			pMap->Prev = pMapFolder->Prev;
			pMap->Next = pMapFolder;
			pMapFolder->Prev->Next = pMap;
			pMapFolder->Prev = pMap;
			if (First)
			{
				pMapFirst = pMap->Prev;
				if(strlen(pMapFirst->Buffer))
					*RetnPointer = (DWORD)pMapFirst;
				else
					*RetnPointer = (DWORD)pMapFirst->Next;
			}


			pMap = Next;
		}
		else
		{
			pMap = pMap->Next;
			isGoMap = FALSE;
		}

l_result:;
	} while ((signed)pMap > 0);


	// 이제 리스트에서 [Up One Level] 를 찾아서 위로 올린다.
	pMapFirst = (PMapList)*RetnPointer;
	pMap = pMapFirst;
	pMapFolder = pMapFirst;
	if (strcmp(pMapFirst->Buffer, "[Up One Level]") != 0)
	{
		// @@@@ 루프 =============================================================
		do
		{
			// 너 어디서 많이본 놈이다?!
			if (strcmp(pMap->Buffer, "[Up One Level]") == 0)
			{
				// ㅇㅋ 위로 꺼지셈
				
				// 폴더를 찾으면 링크를 끊는다.
				if ((signed)pMap->Next > 0)
				{
					pMap->Prev->Next = pMap->Next;
					pMap->Next->Prev = pMap->Prev;
				}
				else // 다음 맴버가 없으면
				{
					pMap->Prev->Next = pMap->Next;
				}

				Next = pMap->Next;
				pMap->Prev = pMapFolder->Prev;
				pMap->Next = pMapFolder;
				pMapFolder->Prev->Next = pMap;
				pMapFolder->Prev = pMap;
				*RetnPointer = (DWORD)pMap;
				break;

			}

			pMap = pMap->Next;
		} while ((signed)pMap > 0);
		// @@@@ 루프 =============================================================
	}
	

}

// 어셈함수
void __declspec(naked) __stdcall HookList()
{
	__asm
	{
		PUSH 0x11223344
		PUSHAD
		MOV EAX,ESP
		ADD EAX, 0x20
		PUSH EAX
		PUSH DWORD PTR DS : [0x51A27C]
		CALL SortList
		POPAD

		POP EAX
		PUSH 0x4A7422
		RETN
	}

}

bool CheckMemory() {
	// Check Memory
	LPVOID lpList = (LPVOID)0x4A741E;

	MEMORY_BASIC_INFORMATION MBI = { 0, };
	VirtualQuery(lpList, &MBI, sizeof(MEMORY_BASIC_INFORMATION));
	{
		if (MBI.State & MEM_COMMIT)
			if (MBI.Type & MEM_IMAGE)
				if (!(MBI.Protect & PAGE_NOACCESS))
				{
					if (*(PDWORD)lpList == 0x51A27C)
					{
						return true;
					}
				}
	}
	return false;
}

LPVOID _Command = (LPVOID)0x4A741D;
BYTE g_OriginalBytes[5];

unsigned int __stdcall ThreadFunc(PVOID pvParam)
{
// 	AllocConsole();
// 
// 	FILE *fRead, *fWrite;
// 	freopen_s(&fRead, "CONOUT$", "rt", stdin);
// 	freopen_s(&fWrite, "CONOUT$", "wt", stdout);
// 	SetConsoleTitle(TEXT("Debug"));
	
	// 메모리 확인 : 엑세스 못하면 뻗어버릴수가 있으니깐.
	if (!CheckMemory()) return -1;

	// 후킹
	// 백업
	memcpy(g_OriginalBytes, _Command, sizeof(g_OriginalBytes));
		
	// 권한을 없엠
	DWORD OldProtect;
	VirtualProtect(_Command, 5, PAGE_EXECUTE_READWRITE, &OldProtect);

	// 얍(?)
	*(PBYTE)(_Command) = 0xE9;
	*(PDWORD)((DWORD)_Command + 1) = (DWORD)HookList - (DWORD)_Command - 5;

	// 복구
	VirtualProtect(_Command, 5, OldProtect, NULL);

	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		_beginthreadex(NULL, NULL, ThreadFunc, NULL, NULL, NULL);	
	 break;
	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		memcpy(_Command, g_OriginalBytes, sizeof(g_OriginalBytes));
		//FreeConsole();
		//h_CreateWindowExA.UnHook();
		break;
	}
	return TRUE;
}

