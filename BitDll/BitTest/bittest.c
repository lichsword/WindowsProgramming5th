#include<windows.h>
#include<tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[] = _T("ShowBit");

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
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Show Bitmaps from BITLIB"),
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

void DrawBitmap(HDC hdc, int xStart, int yStart, HBITMAP hBitmap)
{
	BITMAP bm;
	HDC hMemDC;
	POINT pt;

	hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmap);
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	pt.x = bm.bmWidth;
	pt.y = bm.bmHeight;

	BitBlt(hdc, xStart, yStart, pt.x, pt.y, hMemDC, 0, 0, SRCCOPY);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static HINSTANCE hLibrary;
	static int iCurrent = 1;
	HBITMAP hBitmap;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		if (NULL == (hLibrary = LoadLibrary(_T("Bitlib.dll"))))
		{
			MessageBox(hwnd, _T("Can't load bitlib.dll"), szAppName, 0);
			return -1;
		}
		return 0;

	case WM_CHAR:
		if (NULL != hLibrary)
		{
			iCurrent ++;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		if (NULL != hLibrary)
		{
			hBitmap = LoadBitmap(hLibrary, MAKEINTRESOURCE(iCurrent));

			if (NULL == hBitmap)
			{
				iCurrent = 1;
				hBitmap = LoadBitmap(hLibrary, MAKEINTRESOURCE(iCurrent));
			}
			if (NULL != hBitmap)
			{
				DrawBitmap(hdc, 0, 0, hBitmap);
				DeleteObject(hBitmap);
			}
		}

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		if (NULL != hLibrary)
		{
			FreeLibrary(hLibrary);
		}
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}