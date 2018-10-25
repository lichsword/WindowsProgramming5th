#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("HelloBit");
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
		_T("HelloBit"),
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
	static HBITMAP hBitmap;
	static HDC hdcMem;
	static int cxBitmap, cyBitmap, cxClient, cyClient, iSize = IDM_BIG;
	static TCHAR* szText = _T(" Hello, world! ");
	HDC hdc;
	HMENU hMenu;
	int x, y;
	PAINTSTRUCT ps;
	SIZE size;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC(hdc);
		GetTextExtentPoint32(hdc, szText, _tcslen(szText), &size);
		cxBitmap = size.cx;
		cyBitmap = size.cy;
		hBitmap = CreateCompatibleBitmap(hdc, cxBitmap, cyBitmap);

		ReleaseDC(hwnd, hdc);
		SelectObject(hdcMem, hBitmap);
		TextOut(hdcMem, 0, 0, szText, _tcslen(szText));
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_COMMAND:
		hMenu = GetMenu(hwnd);
		switch (LOWORD(wParam))
		{
		case IDM_BIG:
		case IDM_SMALL:
			CheckMenuItem(hMenu, iSize, MF_UNCHECKED);
			iSize = LOWORD(wParam);
			CheckMenuItem(hMenu, iSize, MF_CHECKED);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		switch (iSize)
		{
		case IDM_BIG:
			StretchBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, cxBitmap, cyBitmap, SRCCOPY);
			break;

		case IDM_SMALL:
			for (y = 0; y < cyClient; y += cyBitmap)
			{
				for (x = 0; x < cxClient; x += cxBitmap)
				{
					BitBlt(hdc, x, y, cxBitmap, cyBitmap, hdcMem, 0, 0, SRCCOPY);
				}
			}
			break;
		}
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
