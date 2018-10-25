#include<windows.h>
#include<tchar.h>
#include "resource.h"

#define ID_TIMER		1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("MenuDemo");

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
		_T("Menu Demonstration"),
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static	int			idColor[5] = {WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH};
	static  int			iSelection = IDM_BKGND_WHITE;
	HMENU				hMenu;

	switch (message)
	{
	case WM_COMMAND:
		hMenu = GetMenu(hwnd);

		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
			MessageBeep(0);
			return 0;

		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		case IDM_EDIT_UNDO:
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
			MessageBeep(0);
			return 0;

		case IDM_BKGND_WHITE:
		case IDM_BKGND_LTGRAY:
		case IDM_BKGND_GRAY:
		case IDM_BKGND_DKGRAY:
		case IDM_BKGND_BLACK:
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);
			iSelection = LOWORD(wParam);
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);

			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case IDM_TIMER_START:
			if (SetTimer(hwnd, ID_TIMER, 1000, NULL))
			{
				EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);
			}
			return 0;

		case IDM_TIMER_STOP:
			KillTimer(hwnd, ID_TIMER);
			EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);
			return 0;

		case IDM_APP_HELP:
			MessageBox(hwnd, _T("Help not yet implemented!"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			return 0;

		case IDM_APP_ABOUT:
			MessageBox(hwnd, _T("Menu Demonstration Program\n(c)Charles Petzold, 1998"), szAppName, MB_ICONINFORMATION | MB_OK);
			return 0;
		}
		break;

	case WM_TIMER:
		MessageBeep(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
