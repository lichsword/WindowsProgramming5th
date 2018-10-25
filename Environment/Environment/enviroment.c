#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define ID_LIST		1
#define ID_TEXT		2

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	TCHAR szAppName[] = _T("Environ");

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Environment List Box"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void FillListBox(HWND hwndList)
{
	int		iLength;
	TCHAR  *pVarBlock, *pVarBeg, *pVarEnd, *pVarName;
	TCHAR  *pVarStart;

	pVarBlock = GetEnvironmentStrings();
	pVarStart = pVarBlock;

	while (*pVarBlock)
	{
		if (*pVarBlock != _T('='))
		{
			pVarBeg = pVarBlock;
			while (*pVarBlock++ != _T('='))
			{

			}
			pVarEnd = pVarBlock - 1;
			iLength = pVarEnd - pVarBeg;

			pVarName = calloc(iLength + 1, sizeof(TCHAR));
			CopyMemory(pVarName, pVarBeg, iLength * sizeof(TCHAR));
			pVarName[iLength] = _T('\0');

			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)pVarName);
			free(pVarName);
		}
		while(*pVarBlock++ != _T('\0'));
	}

	FreeEnvironmentStrings(pVarStart);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndList, hwndText;
	int			iIndex, iLength, cxChar, cyChar;
	TCHAR		*pVarName, *pVarValue;

	switch (message)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		hwndList = CreateWindow(_T("listbox"), NULL,
			WS_CHILD | WS_VISIBLE | LBS_STANDARD,
			cxChar, cyChar * 3, cxChar * 16 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 5,
			hwnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		hwndText = CreateWindow(_T("static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 
			cxChar, cyChar, GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, (HMENU)ID_TEXT, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		FillListBox(hwndList);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE)
		{
			iIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			iLength = SendMessage(hwndList, LB_GETTEXTLEN, iIndex, 0) + 1;
			pVarName = calloc(iLength, sizeof(TCHAR));
			SendMessage(hwndList, LB_GETTEXT, iIndex, (LPARAM)pVarName);

			iLength = GetEnvironmentVariable(pVarName, NULL, 0);
			pVarValue = calloc(iLength, sizeof(TCHAR));
			GetEnvironmentVariable(pVarName, pVarValue, iLength);

			SetWindowText(hwndText, pVarValue);
			free(pVarName);
			free(pVarValue);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
