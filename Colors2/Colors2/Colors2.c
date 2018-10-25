#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK ColorScrDlg(HWND, UINT, WPARAM, LPARAM);

HWND hDlgModeLess;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Colors2");
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
	wndClass.hbrBackground = CreateSolidBrush(0);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Color Scroll"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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

	hDlgModeLess = CreateDialog(hInstance, _T("ColorScrDlg"), hWnd, ColorScrDlg);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hDlgModeLess == NULL || !IsDialogMessage(hDlgModeLess, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		DeleteObject((HGDIOBJ)SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK ColorScrDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static	int		iColor[3];
	HWND			hwndParent, hCtrl;
	int				iCtrlID, iIndex;

	switch (message)
	{
	case WM_INITDIALOG:
		for (iCtrlID = IDC_SCROLL_RED; iCtrlID < IDC_SCROLL_BLUE + 1; iCtrlID++)
		{
			hCtrl = GetDlgItem(hDlg, iCtrlID);
			SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE);
			SetScrollPos(hCtrl, SB_CTL, 0, FALSE);
		}
		return TRUE;

	case WM_VSCROLL:
		hCtrl = (HWND)lParam;
		iCtrlID = GetWindowLong(hCtrl, GWL_ID);
		iIndex = iCtrlID - IDC_SCROLL_RED;
		hwndParent = GetParent(hDlg);

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			iColor[iIndex] += 15;
			//fall through
		case SB_LINEDOWN:
			iColor[iIndex] = min(255, iColor[iIndex] + 1);
			break;
		case SB_PAGEUP:
			iColor[iIndex] -= 15;
			//fall through
		case SB_LINEUP:
			iColor[iIndex] = max(0, iColor[iIndex] - 1);
			break;
		case SB_TOP:
			iColor[iIndex] = 0;
			break;
		case SB_BOTTOM:
			iColor[iIndex] = 255;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			iColor[iIndex] = HIWORD(wParam);
			break;
		default:
			return FALSE;
		}

		SetScrollPos(hCtrl, SB_CTL, iColor[iIndex], TRUE);
		SetDlgItemInt(hDlg, IDC_TEXT_RED + iCtrlID - IDC_SCROLL_RED, iColor[iIndex], FALSE);
		DeleteObject((HGDIOBJ)SetClassLong(hwndParent, GCL_HBRBACKGROUND, 
			(LONG)CreateSolidBrush(RGB(iColor[0], iColor[1], iColor[2]))));
		InvalidateRect(hwndParent, NULL, TRUE);
		return TRUE;
	}
	
	return FALSE;
}