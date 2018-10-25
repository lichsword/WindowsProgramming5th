#include<windows.h>
#include<tchar.h>
#include "strlib.h"

#pragma data_seg("shared")
int iTotal = 0;
TCHAR szStrings[MAX_STRINGS][MAX_LENGTH + 1] = {_T('\0')};
#pragma data_seg()

#pragma comment(linker, "/SECTION:shared,RWS")

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	return TRUE;
}

EXPORT BOOL CALLBACK AddString(PCTSTR pStringIn)
{
	PTSTR pString;
	int i, iLength;

	if (MAX_STRINGS - 1 == iTotal)
	{
		return FALSE;
	}
	if (0 == (iLength = _tcslen(pStringIn)))
	{
		return FALSE;
	}

	pString = malloc(sizeof(TCHAR) * (1 + iLength));
	_tcscpy(pString, pStringIn);
	_tcsupr(pString);

	for (i = iTotal; i > 0; i--)
	{
		if (_tcscmp(pString, szStrings[i - 1]) >= 0)
		{
			break;
		}
		_tcscpy(szStrings[i], szStrings[i - 1]);
	}
	_tcscpy(szStrings[i], pString);
	iTotal++;

	free(pString);
	return TRUE;
}

EXPORT BOOL CALLBACK DeleteString(PCTSTR pStringIn)
{
	int i, j;

	if (0 == _tcslen(pStringIn))
	{
		return FALSE;
	}

	for (i = 0; i < iTotal; i++)
	{
		if (_tcsicmp(szStrings[i], pStringIn) == 0)
		{
			break;
		}
	}

	if (i == iTotal)
	{
		return FALSE;
	}
	for (j = i; j < iTotal; j++)
	{
		_tcscpy(szStrings[j], szStrings[j + 1]);
	}

	szStrings[iTotal--][0] = '\0';
	return TRUE;
}

EXPORT int CALLBACK GetStrings(GetStrCb pfnGetStrCallBack, PVOID pParam)
{
	BOOL bReturn;
	int i;

	for (i = 0; i < iTotal; i++)
	{
		bReturn = pfnGetStrCallBack(szStrings[i], pParam);
		if (!bReturn)
		{
			return i + 1;
		}
	}

	return iTotal;
}