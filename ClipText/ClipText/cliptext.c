#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define CF_TCHAR	CF_UNICODETEXT
TCHAR	szDefaultText[] = _T("Default Text - Unicode Version");
TCHAR	szCaption[] = _T("Clipboard Text transfers - Unicode Version");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("ClipText");
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
		szCaption,
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
	static PTSTR		pText;
	BOOL				bEnable;
	HGLOBAL				hGlobal;
	HDC					hdc;
	PTSTR				pGlobal;
	PAINTSTRUCT			ps;
	RECT				rect;

	switch (message)
	{
	case WM_CREATE:
		SendMessage(hwnd, WM_COMMAND, IDM_EDIT_RESET, 0);
		return 0;

	case WM_INITMENUPOPUP:
		EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TCHAR) ? MF_ENABLED : MF_GRAYED);
		bEnable = (NULL != pText )? MF_ENABLED : MF_GRAYED;
		EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, bEnable);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EDIT_PASTE:
			OpenClipboard(hwnd);

			if (NULL != (hGlobal = GetClipboardData(CF_TCHAR)))
			{
				pGlobal = GlobalLock(hGlobal);
				if (NULL != pText)
				{
					free(pText);
					pText = NULL;
				}
				pText = malloc(GlobalSize(hGlobal));
				_tcscpy(pText, pGlobal);
				InvalidateRect(hwnd, NULL, TRUE);
			}
			CloseClipboard();
			return 0;

		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
			if (NULL == pText)
			{
				return 0;
			}
			hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (_tcslen(pText) + 1) * sizeof(TCHAR));
			pGlobal = GlobalLock(hGlobal);
			_tcscpy(pGlobal, pText);
			GlobalUnlock(hGlobal);
			
			OpenClipboard(hwnd);
			EmptyClipboard();
			SetClipboardData(CF_TCHAR, hGlobal);
			CloseClipboard();

			if (LOWORD(wParam) == IDM_EDIT_COPY)
			{
				return 0;
			}

			//fall throuth for IDM_EDIT_CUT

		case IDM_EDIT_CLEAR:
			if (NULL != pText)
			{
				free(pText);
				pText = NULL;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case IDM_EDIT_RESET:
			if (NULL != pText)
			{
				free(pText);
				pText = NULL;
			}
			pText = malloc((_tcslen(szDefaultText) + 1) * sizeof(TCHAR));
			_tcscpy(pText, szDefaultText);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);

		if (NULL != pText)
		{
			DrawText(hdc, pText, -1, &rect, DT_EXPANDTABS | DT_WORDBREAK);
		}

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		if (NULL != pText)
		{
			free(pText);
			pText = NULL;
		}
		DeleteObject((HGDIOBJ)SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

