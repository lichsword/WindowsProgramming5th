#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Bricks1");
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
		_T("LoadBitmap Demo"),
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
	static BITMAP bitmap = {0, 8, 8, 2, 1, 1};
	static BYTE bits[8][2] = {0xFF, 0, 0x0C, 0, 0x0C, 0, 0x0C, 0,
		0xFF, 0, 0xC0, 0, 0xC0, 0, 0xC0, 0};

	static HBITMAP hBitmap;
	static int cxClient, cyClient, cxSource, cySource;
	HDC hdc, hdcMem;
	int x, y;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		bitmap.bmBits = bits;
		hBitmap = CreateBitmapIndirect(&bitmap);
		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight;
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);

		for (y = 0; y < cyClient; y += cySource)
		{
			for (x = 0; x < cxClient; x += cxSource)
			{
				BitBlt(hdc, x, y, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
			}
		}

		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
