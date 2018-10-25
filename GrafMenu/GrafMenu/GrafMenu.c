#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddHelpToSys(HINSTANCE, HWND);
HMENU CreateMyMenu(HINSTANCE);
HBITMAP StretchBitmap(HBITMAP);
HBITMAP GetBitmapFont(int);
void DeleteAllBitmaps(HWND);

TCHAR szAppName[] = _T("GrafMenu");

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
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Bitmap Menu Demonstration"),
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	HMENU	hMenu;
	static int iCurrentFont = IDM_FONT_COUR;

	switch (message)
	{
	case WM_CREATE:
		AddHelpToSys(((LPCREATESTRUCT)lParam)->hInstance, hwnd);
		hMenu = CreateMyMenu(((LPCREATESTRUCT)lParam)->hInstance);
		SetMenu(hwnd, hMenu);
		CheckMenuItem(hMenu, iCurrentFont, MF_CHECKED);
		return 0;

	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_HELP:
			MessageBox(hwnd, _T("Hlep not yet implemented!"), szAppName, MB_OK | MB_ICONEXCLAMATION);
			return 0;
		}
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		case IDM_EDIT_UNDO:
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
			MessageBeep(0);
			return 0;

		case IDM_FONT_COUR:
		case IDM_FONT_ARIAL:
		case IDM_FONT_TIMES:
			hMenu = GetMenu(hwnd);
			CheckMenuItem(hMenu, iCurrentFont, MF_UNCHECKED);
			iCurrentFont = LOWORD(wParam);
			CheckMenuItem(hMenu, iCurrentFont, MF_CHECKED);
			return 0;
		}
		break;

	case WM_DESTROY:
		DeleteAllBitmaps(hwnd);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void AddHelpToSys(HINSTANCE hIntance, HWND hwnd)
{
	HBITMAP hBitmap;
	HMENU hMenu;

	hMenu = GetSystemMenu(hwnd, FALSE);
	hBitmap = StretchBitmap(LoadBitmap(hIntance, _T("BigHelp")));
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_BITMAP, IDM_HELP, (PTSTR)(LONG)hBitmap);
}

HMENU CreateMyMenu(HINSTANCE hInstance)
{
	HBITMAP hBitmap;
	HMENU hMenu, hMenuPopup;
	int i;

	hMenu = CreateMenu();
	hMenuPopup = LoadMenu(hInstance, _T("MenuFile"));
	hBitmap = StretchBitmap(LoadBitmap(hInstance, _T("FILELABEL")));
	AppendMenu(hMenu, MF_BITMAP | MF_POPUP, (int)hMenuPopup, (PTSTR)(LONG)hBitmap);

	hMenuPopup = LoadMenu(hInstance, _T("MenuEdit"));
	hBitmap = StretchBitmap(LoadBitmap(hInstance, _T("EDITLABEL")));
	AppendMenu(hMenu, MF_BITMAP | MF_POPUP, (int)hMenuPopup, (PTSTR)(LONG)hBitmap);

	hMenuPopup = CreateMenu();
	for (i = 0; i < 3; i++)
	{
		hBitmap = GetBitmapFont(i);
		AppendMenu(hMenuPopup, MF_BITMAP, IDM_FONT_COUR + i, (PTSTR)(LONG)hBitmap);
	}
	hBitmap = StretchBitmap(LoadBitmap(hInstance, _T("FONTLABEL")));
	AppendMenu(hMenu, MF_BITMAP | MF_POPUP, (int)hMenuPopup, (PTSTR)(LONG)hBitmap);

	return hMenu;
}

HBITMAP StretchBitmap(HBITMAP hBitmap1)
{
	BITMAP bm1, bm2;
	HBITMAP hBitmap2;
	HDC hdc, hdcMem1, hdcMem2;
	int cxChar, cyChar;

	cxChar = LOWORD(GetDialogBaseUnits());
	cyChar = HIWORD(GetDialogBaseUnits());

	hdc = CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	hdcMem1 = CreateCompatibleDC(hdc);
	hdcMem2 = CreateCompatibleDC(hdc);
	DeleteDC(hdc);

	GetObject(hBitmap1, sizeof(BITMAP), (PTSTR)&bm1);

	bm2 = bm1;
	bm2.bmWidth = (cxChar * bm2.bmWidth) / 4;
	bm2.bmHeight = (cyChar * bm2.bmHeight) / 8;
	bm2.bmWidthBytes = ((bm2.bmWidthBytes + 15) / 16) * 32;

	hBitmap2 = CreateBitmapIndirect(&bm2);
	SelectObject(hdcMem1, hBitmap1);
	SelectObject(hdcMem2, hBitmap2);

	StretchBlt(hdcMem2, 0, 0, bm2.bmWidth, bm2.bmHeight, hdcMem1, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY);
	DeleteDC(hdcMem1);
	DeleteDC(hdcMem2);
	DeleteObject(hBitmap1);

	return hBitmap2;
}

HBITMAP GetBitmapFont(int i)
{
	static TCHAR* szFaceName[3] = {_T("Courier New"), _T("Arial"), _T("Times New Roman")};
	HBITMAP hBitmap;
	HDC hdc, hdcMem;
	HFONT hFont;
	SIZE size;
	TEXTMETRIC tm;

	hdc = CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	GetTextMetrics(hdc, &tm);

	hdcMem = CreateCompatibleDC(hdc);
	hFont = CreateFont(2 * tm.tmHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, szFaceName[i]);
	hFont = (HFONT)SelectObject(hdcMem, hFont);
	GetTextExtentPoint32(hdcMem, szFaceName[i], _tcslen(szFaceName[i]), &size);

	hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, NULL);
	SelectObject(hdcMem, hBitmap);

	TextOut(hdcMem, 0, 0, szFaceName[i], _tcslen(szFaceName[i]));
	
	DeleteObject(SelectObject(hdcMem, hFont));

	DeleteDC(hdcMem);
	DeleteDC(hdc);

	return hBitmap;
}

void DeleteAllBitmaps(HWND hwnd)
{
	HMENU hMenu;
	int i;
	MENUITEMINFO mii = {sizeof(MENUITEMINFO), MIIM_SUBMENU | MIIM_TYPE};

	hMenu = GetSystemMenu(hwnd, FALSE);
	GetMenuItemInfo(hMenu, IDM_HELP, FALSE, &mii);
	DeleteObject((HBITMAP)mii.dwTypeData);

	hMenu = GetMenu(hwnd);
	for (i = 0; i < 3; i++)
	{
		GetMenuItemInfo(hMenu, i, TRUE, &mii);
		DeleteObject((HBITMAP)mii.dwTypeData);
	}

	hMenu = mii.hSubMenu;
	for (i = 0;i < 3; i++)
	{
		GetMenuItemInfo(hMenu, i, TRUE, &mii);
		DeleteObject((HBITMAP)mii.dwTypeData);
	}
}