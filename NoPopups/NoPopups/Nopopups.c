#include<windows.h>
#include<tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] =_T("NoPopUps");
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
		_T("No-Popup Nested Menu Demonstration"),
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
	static HMENU		hMenuMain, hMenuEdit, hMenuFile;
	HINSTANCE			hInstance;
	switch (message)
	{
	case WM_CREATE:
		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		hMenuMain = LoadMenu(hInstance, _T("MenuMain"));
		hMenuFile = LoadMenu(hInstance, _T("MenuFile"));
		hMenuEdit = LoadMenu(hInstance, _T("MenuEdit"));

		SetMenu(hwnd, hMenuMain);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_MAIN:
			SetMenu(hwnd, hMenuMain);
			return 0;

		case IDM_FILE:
			SetMenu(hwnd, hMenuFile);
			return 0;

		case IDM_EDIT:
			SetMenu(hwnd, hMenuEdit);
			return 0;

		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		case IDM_EDIT_UNDO:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
			MessageBeep(0);
			return 0;
		}
		break;

	case WM_DESTROY:
		SetMenu(hwnd, hMenuMain);
		DestroyMenu(hMenuFile);
		DestroyMenu(hMenuEdit);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
