#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("UniChars");
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
		_T("Unicode Characters"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
	static int iPage;
	static LOGFONT lf;
	HDC hdc;

	int cxChar, cyChar, x, y, i, cxLabels;
	PAINTSTRUCT ps;
	SIZE size;
	TCHAR szBuffer[8];
	TEXTMETRIC tm;
	TCHAR ch;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		lf.lfWeight = -GetDeviceCaps(hdc, LOGPIXELSY) / 6;
		_tcscpy(lf.lfFaceName, _T("Lucida Sans Unicode"));
		ReleaseDC(hwnd, hdc);

		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.hDC = NULL;
		cf.lpLogFont = &lf;
		cf.iPointSize = 0;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
		cf.rgbColors = 0;
		cf.lCustData = 0;
		cf.lpfnHook = NULL;
		cf.lpTemplateName = NULL;
		cf.hInstance = NULL;
		cf.lpszStyle = NULL;
		cf.nFontType = 0;
		cf.nSizeMin = 0;
		cf.nSizeMax = 0;
		
		SetScrollRange(hwnd, SB_VERT, 0, 255, FALSE);
		SetScrollPos(hwnd, SB_VERT, iPage, TRUE);

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

	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iPage -= 1;
			break;

		case SB_LINEDOWN:
			iPage += 1;
			break;

		case SB_PAGEUP:
			iPage -= 16;
			break;

		case SB_PAGEDOWN:
			iPage += 16;
			break;

		case SB_THUMBPOSITION:
			iPage = HIWORD(wParam);
			break;

		default:
			return 0;
		}
		iPage = max(0, min(iPage, 255));
		SetScrollPos(hwnd, SB_VERT, iPage, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, CreateFontIndirect(&lf));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmMaxCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		
		cxLabels = 0;

		for (i = 0; i < 16; i++)
		{
			_stprintf(szBuffer, _T(" 000%1X: "), i);
			GetTextExtentPoint(hdc, szBuffer, 7, &size);

			cxLabels = max(cxLabels, size.cx);
		}
		for (y = 0; y < 16; y++)
		{
			_stprintf(szBuffer, _T(" %03X_: "), 16 * iPage + y);
			TextOut(hdc, 0, y * cyChar, szBuffer, 7);

			for (x = 0; x < 16; x++)
			{
				ch = (WCHAR)(256 * iPage + 16 * y + x);
				TextOut(hdc, x * cxChar + cxLabels, y * cyChar, &ch, 1);
			}
		}

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}