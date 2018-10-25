#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define CF_TCHAR	CF_UNICODETEXT

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T(".ClipView.");
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
		_T("Simple Clipboard Viewer (Text Only)"),
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
	static HWND			hwndNextViewer;
	HGLOBAL				hGlobal;
	HDC					hdc;
	PTSTR				pGlobal;
	PAINTSTRUCT			ps;
	RECT				rect;

	switch (message)
	{
	case WM_CREATE:
		hwndNextViewer = SetClipboardViewer(hwnd);
		return 0;

	case WM_CHANGECBCHAIN:
		//delete wParam in the list chain
		if ((HWND)wParam == hwndNextViewer)
		{
			hwndNextViewer = (HWND)lParam;
		}
		else if (NULL != hwndNextViewer)
		{
			SendMessage(hwndNextViewer, message, wParam, lParam);
		}
		return 0;

	case WM_DRAWCLIPBOARD:
		if (NULL != hwndNextViewer)
		{
			SendMessage(hwndNextViewer, message, wParam, lParam);
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		OpenClipboard(hwnd);

		hGlobal = GetClipboardData(CF_TCHAR);
		if (NULL != hGlobal)
		{
			pGlobal = (PTSTR)GlobalLock(hGlobal);
			DrawText(hdc, pGlobal, -1, &rect, DT_EXPANDTABS);
			GlobalUnlock(hGlobal);
		}
		CloseClipboard();
		EndPaint(hwnd, &ps);

		return 0;

	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNextViewer);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

