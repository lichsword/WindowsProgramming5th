#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("ChosFont");
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
		_T("ChooseFont"),
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

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static CHOOSEFONT cf;
	static int cyChar;
	static LOGFONT lf;
	static TCHAR szText[] = _T("\x41\x42\x43\x44\x45 ")
		_T("\x61\x62\x63\x64\x65 ")
		_T("\xC0\xC1\xC2\xC3\xC4\xC5 ")
		_T("\xE0\xE1\xE2\xE3\xE4\xE5 ")
		_T("\x0390\x0391\x0392\x0393\x0394\x0395 ")
		_T("\x03B0\x03B1\x03B2\x03B3\x03B4\x03B5 ")
		_T("\x0410\x0411\x0412\x0413\x0414\x0415 ")
		_T("\x0430\x0431\x0432\x0433\x0434\x0435 ")
		_T("\x5000\x5001\x5002\x5003\x5004");
	HDC hdc;
	int y;
	PAINTSTRUCT ps;
	TCHAR szBuffer[64];
	TEXTMETRIC tm;

	switch (message)
	{
	case WM_CREATE:
		cyChar = HIWORD(GetDialogBaseUnits());
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
		
		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.hDC = NULL;
		cf.lpLogFont = &lf;
		cf.iPointSize = 0;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
		cf.rgbColors = 0;
		cf.lCustData = 0;
		cf.lpfnHook = NULL;
		cf.lpTemplateName = NULL;
		cf.hInstance = NULL;
		cf.lpszStyle = NULL;
		cf.nFontType = 0;
		cf.nSizeMin = 0;
		cf.nSizeMax = 0;
		return 0;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FONT:
			if (ChooseFont(&cf))
			{
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, CreateFontIndirect(&lf));
		GetTextMetrics(hdc, &tm);
		SetTextColor(hdc, cf.rgbColors);
		TextOut(hdc, 0, y = tm.tmExternalLeading, szText, _tcslen(szText));

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		SetTextColor(hdc, 0);

		TextOut(hdc, 0, y += tm.tmHeight, szBuffer, 
			_stprintf(szBuffer, _T("lfHeight = %i"), lf.lfHeight));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfWidth = %i"), lf.lfWidth));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfEscapement = %i"), lf.lfEscapement));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfOrientation = %i"), lf.lfOrientation));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfWeight = %i"), lf.lfWeight));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfItalic = %i"), lf.lfItalic));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfUnderline = %i"), lf.lfUnderline));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfStrikeOut = %i"), lf.lfStrikeOut));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfCharSet = %i"), lf.lfCharSet));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfOutPrecision = %i"), lf.lfOutPrecision));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfClipPrecision = %i"), lf.lfClipPrecision));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfQuality = %i"), lf.lfQuality));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfPitchAndFamily = 0x%02X"), lf.lfPitchAndFamily));
		TextOut(hdc, 0, y += cyChar, szBuffer, 
			_stprintf(szBuffer, _T("lfFaceName = %s"), lf.lfFaceName));

		EndPaint(hwnd, &ps);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}