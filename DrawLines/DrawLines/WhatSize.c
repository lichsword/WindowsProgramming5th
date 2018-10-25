#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("WhatSize");
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
		_T("What Size is the Window?"),
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

void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode, TCHAR* szMapMode)
{
	TCHAR szBuffer[60];
	RECT rect;

	SaveDC(hdc);
	SetMapMode(hdc, iMapMode);
	GetClientRect(hwnd, &rect);
	DPtoLP(hdc, (PPOINT)&rect, 2);
	RestoreDC(hdc, -1);

	TextOut(hdc, xText, yText, szBuffer, 
		_stprintf(szBuffer, _T("%-20s %7d %7d %7d %7d"), szMapMode, rect.left, rect.right, rect.top, rect.bottom)
		);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR szHeading[] = _T("Mapping Mode              Left  Right     Top  Bottom");
	static TCHAR szUndLine[] = _T("------------              ---   ----      ---  ------");
	static int cxChar, cyChar;
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC	tm;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 1, 1, NULL);
		SetViewportExtEx(hdc, cxChar, cyChar, NULL);

		TextOut(hdc, 1, 1, szHeading, _tcslen(szHeading));
		TextOut(hdc, 1, 2, szUndLine, _tcslen(szUndLine));

		Show(hwnd, hdc, 1, 3, MM_TEXT,		_T("Text(pixels)"));
		Show(hwnd, hdc, 1, 4, MM_LOMETRIC,	_T("LOMETRIC(.1 mm)"));
		Show(hwnd, hdc, 1, 5, MM_HIMETRIC,	_T("HIMETRIC(.01 mm)"));
		Show(hwnd, hdc, 1, 6, MM_LOENGLISH,	_T("LOENGLISH(.01 in)"));
		Show(hwnd, hdc, 1, 7, MM_HIENGLISH, _T("HIENGLISH(.001 in)"));
		Show(hwnd, hdc, 1, 8, MM_TWIPS,		_T("TWIPS(1/1440 in)"));

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}


	return DefWindowProc(hwnd, message, wParam, lParam);
}