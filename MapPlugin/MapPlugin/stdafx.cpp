// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// MapPlugin.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.

void MyMessageBox(char* pFormat, ...)
{
	char szData[256];
	va_list argptr;
	va_start(argptr, pFormat);
	vsprintf_s(szData,256, pFormat, argptr);
	va_end(argptr);
	MessageBoxA(NULL, szData, "TITLE", MB_OK);
}