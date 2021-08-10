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

// ��Ÿ ������ ����ü
typedef struct tagMapList {
	tagMapList* Prev;
	tagMapList* Next;
	char Buffer[100];
} *PMapList;


// �����Լ� ��������Ʈ�� ��ȸ�ؼ� �����Ѵ�.
void __stdcall SortList(DWORD Pointer , DWORD *RetnPointer) {

	PMapList pMapFirst = (PMapList)Pointer;
	PMapList pMap;			// ��ȸ�� �ɹ�
	PMapList pMapFolder;	// ������ �ִ°�
	tagMapList* Next;

	pMap = pMapFirst;

	DWORD ItemCount = 0;
	DWORD i = 0;
	BOOL isGoMap = TRUE;
	BOOL First = TRUE;

	*RetnPointer = (DWORD)pMapFirst;

	// ��ü �������� 2�� ���ϸ� ������ �ʿ���� ����
	if ((signed)pMap <= 0) return;
	do
	{
		ItemCount++;
		pMap = pMap->Next;
	} while ((signed)pMap > 0);
	if (ItemCount < 6) return;

	// pMapFirst = ����Ʈ ó�� ����
	// pMapFolder = �� ó�� ����
	pMap = pMapFirst;
	pMapFolder = pMapFirst;

	do 
	{
		// �������̸�
		if (pMap->Buffer[lstrlenA(pMap->Buffer) - 1] == ']')
		{
			// ���� ������ ������ ã�� (�ű⼭���� �����Ҳ���)
			// ���� �����°��� ã��
			if (isGoMap)
			{
				First = FALSE;
				pMapFolder = pMap->Next;
				pMap = pMap->Next;
				// ������ �ݺ��ϴ� ������ ������ ����
				if ((signed)pMapFolder <= 0) break;
				goto l_result;
			}
			

			// ������ ã���� ��ũ�� ���´�.
			if ((signed)pMap->Next > 0)
			{
				pMap->Prev->Next = pMap->Next;
				pMap->Next->Prev = pMap->Prev;
			}
			else // ���� �ɹ��� ������
			{
				pMap->Prev->Next = pMap->Next;
			}

			// pMapFolder �� �ɹ��� ����
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


	// ���� ����Ʈ���� [Up One Level] �� ã�Ƽ� ���� �ø���.
	pMapFirst = (PMapList)*RetnPointer;
	pMap = pMapFirst;
	pMapFolder = pMapFirst;
	if (strcmp(pMapFirst->Buffer, "[Up One Level]") != 0)
	{
		// @@@@ ���� =============================================================
		do
		{
			// �� ��� ���̺� ���̴�?!
			if (strcmp(pMap->Buffer, "[Up One Level]") == 0)
			{
				// ���� ���� ������
				
				// ������ ã���� ��ũ�� ���´�.
				if ((signed)pMap->Next > 0)
				{
					pMap->Prev->Next = pMap->Next;
					pMap->Next->Prev = pMap->Prev;
				}
				else // ���� �ɹ��� ������
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
		// @@@@ ���� =============================================================
	}
	

}

// ����Լ�
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
	
	// �޸� Ȯ�� : ������ ���ϸ� ����������� �����ϱ�.
	if (!CheckMemory()) return -1;

	// ��ŷ
	// ���
	memcpy(g_OriginalBytes, _Command, sizeof(g_OriginalBytes));
		
	// ������ ����
	DWORD OldProtect;
	VirtualProtect(_Command, 5, PAGE_EXECUTE_READWRITE, &OldProtect);

	// ��(?)
	*(PBYTE)(_Command) = 0xE9;
	*(PDWORD)((DWORD)_Command + 1) = (DWORD)HookList - (DWORD)_Command - 5;

	// ����
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

