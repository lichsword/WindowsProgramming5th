#include<windows.h>
#include<tchar.h>
#include<math.h>

#define ID_TIMER 1
#define TWOPI	(2 * 3.14159)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void FindWindowSize(int*, int*);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("WhatColor");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	int cxWindow, cyWindow;

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

	FindWindowSize(&cxWindow, &cyWindow);

	hWnd = CreateWindow(szAppName,
		_T("What Color"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		cxWindow,
		cyWindow,
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

void FindWindowSize(int* pcxWindow, int* pcyWindow)
{
	HDC			hdcScreen;
	TEXTMETRIC	tm;

	hdcScreen = CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	GetTextMetrics(hdcScreen, &tm);
	DeleteDC(hdcScreen);

	*pcxWindow = 2 * GetSystemMetrics(SM_CXBORDER) + 12 * tm.tmAveCharWidth;
	*pcyWindow = 2 * GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYCAPTION) + 2 * tm.tmHeight;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COLORREF			cr, crLast;
	static HDC				hdcScreen;
	HDC						hdc;
	PAINTSTRUCT				ps;
	POINT					pt;
	RECT					rc;
	TCHAR					szBuffer[32];

	switch (message)
	{
	case WM_CREATE:
		hdcScreen = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		SetTimer(hwnd, ID_TIMER, 1000, NULL);
		return 0;

	case WM_TIMER:
		GetCursorPos(&pt);
		cr = GetPixel(hdcScreen, pt.x, pt.y);
		SetPixel(hdcScreen, pt.x, pt.y, 0);
		if (cr != crLast)
		{
			crLast = cr;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rc);
		_stprintf(szBuffer, _T("    %02X %02X %02X    "), GetRValue(cr), GetGValue(cr), GetBValue(cr));
		DrawText(hdc, szBuffer, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteDC(hdcScreen);
		KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}