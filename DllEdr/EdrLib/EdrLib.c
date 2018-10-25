#include<windows.h>
#include<tchar.h>
#include "EdrLib.h"

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserived)
{
	return TRUE;
}

EXPORT BOOL CALLBACK EdrCenterText(HDC hdc, PRECT prc, PTSTR pString)
{
	int iLength;
	SIZE size;

	iLength = _tcslen(pString);
	GetTextExtentPoint32(hdc, pString, iLength, &size);

	return TextOut(hdc, (prc->right - prc->left - size.cx) / 2,
		(prc->bottom - prc->top - size.cy) / 2, pString, iLength);
}