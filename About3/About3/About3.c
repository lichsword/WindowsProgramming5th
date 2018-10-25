#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("About3");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, szAppName);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = szAppName;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = EllipPushWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = _T("EllipPush");
	RegisterClass(&wndClass);

	hWnd = CreateWindow(szAppName,
		_T("About Box Demo Program"),
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
	static HINSTANCE	hInstance;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			if (IDOK == DialogBox(hInstance, _T("AboutBox"), hwnd, AboutDlgProc))
			{
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND		hCtrlBlock;
	static int		iColor, iFigure;

	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK EllipPushWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR		szText[40];
	HBRUSH		hBrush;
	HDC			hdc;
	PAINTSTRUCT ps;
	RECT		rect;
	
	switch (message)
	{
	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		GetWindowText(hwnd, szText, sizeof(szText));

		hdc = BeginPaint(hwnd, &ps);

		hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		hBrush = (HBRUSH)SelectObject(hdc, hBrush);
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
		DrawText(hdc, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DeleteObject(SelectObject(hdc, hBrush));

		EndPaint(hwnd, &ps);
		return 0;

	case WM_KEYUP:
		if (wParam != VK_SPACE)
		{
			break;
		}
		//fall throuth

	case WM_LBUTTONUP:
		SendMessage(GetParent(hwnd), WM_COMMAND, GetWindowLong(hwnd, GWL_ID), (LPARAM)hwnd);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}