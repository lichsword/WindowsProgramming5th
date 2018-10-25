#include<windows.h>
#include<tchar.h>
#include "../StrLib/strlib.h"
#include "resource.h"

typedef struct 
{
	HDC hdc;
	int xText;
	int yText;
	int xStart;
	int yStart;
	int xIncr;
	int yIncr;
	int xmax;
	int yMax;
}CbParam;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[] = _T("StrProg");
TCHAR szString[MAX_LENGTH + 1];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = szAppName;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("DLL Demonstration Program"),
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

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_STRING, EM_LIMITTEXT, MAX_LENGTH, 0);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_STRING, szString, MAX_LENGTH);
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CALLBACK GetStrCallBack(PTSTR pString, CbParam* pcbp)
{
	TextOut(pcbp->hdc, pcbp->xText, pcbp->yText, pString, _tcslen(pString));

	if ((pcbp->yText += pcbp->yIncr) > pcbp->yMax)
	{
		pcbp->yText = pcbp->yStart;
		if ((pcbp->xText += pcbp->xIncr) > pcbp->xmax)
		{
			return FALSE;
		}
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static HINSTANCE hInst;
	static int cxChar, cyChar, cxClient, cyClient;
	static UINT iDataChangeMsg;
	CbParam cbparam;
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC  tm;

	switch (message)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar = (int)tm.tmAveCharWidth;
		cyChar = (int)(tm.tmHeight + tm.tmExternalLeading);
		ReleaseDC(hwnd, hdc);

		iDataChangeMsg = RegisterWindowMessage(_T("StrProgDataChange"));
		return 0;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_ENTER:
			if (DialogBox(hInst, _T("EnterDlg"), hwnd, &DlgProc))
			{
				if (AddString(szString))
				{
					PostMessage(HWND_BROADCAST, iDataChangeMsg, 0, 0);
				}
				else
				{
					MessageBeep(0);
				}
			}
			break;
		case IDM_DELETE:
			if (DialogBox(hInst, _T("DeleteDlg"), hwnd, &DlgProc))
			{
				if (DeleteString(szString))
				{
					PostMessage(HWND_BROADCAST, iDataChangeMsg, 0, 0);
				}
				else
				{
					MessageBeep(0);
				}
			}
			break;
		}
		return 0;

	case WM_SIZE:
		cxClient = (int)LOWORD(lParam);
		cyClient = (int)HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		cbparam.hdc = hdc;
		cbparam.xText = cbparam.xStart = cxChar;
		cbparam.yText = cbparam.yStart = cyChar;
		cbparam.xIncr = cxChar * MAX_LENGTH;
		cbparam.yIncr = cyChar;
		cbparam.xmax = cbparam.xIncr * (1 + cxClient / cbparam.xIncr);
		cbparam.yMax = cyChar * (cyClient / cyChar - 1);

		GetStrings((GetStrCb)GetStrCallBack, (PVOID)&cbparam);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		if (iDataChangeMsg == message)
		{
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}