#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("BitmapMask");
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
	wndClass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Bitmap Masking Demo"),
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
	static HBITMAP hBitmapImag, hBitmapMask;
	static HINSTANCE hInstance;
	BITMAP bitmap;
	static int cxBitmap, cyBitmap, cxClient, cyClient;
	HDC hdc, hdcMemImag, hdcMemMask;
	int x, y;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

		hBitmapImag = LoadBitmap(hInstance, _T("Matthew"));
		GetObject(hBitmapImag, sizeof(BITMAP), &bitmap);
		cxBitmap = bitmap.bmWidth;
		cyBitmap = bitmap.bmHeight;

		hdcMemImag = CreateCompatibleDC(NULL);
		SelectObject(hdcMemImag, hBitmapImag);

		hBitmapMask = CreateBitmap(cxBitmap, cyBitmap, 1, 1, NULL);
		hdcMemMask = CreateCompatibleDC(NULL);
		SelectObject(hdcMemMask, hBitmapMask);

		SelectObject(hdcMemMask, GetStockObject(BLACK_BRUSH));
		Rectangle(hdcMemMask, 0, 0, cxBitmap, cyBitmap);
		SelectObject(hdcMemMask, GetStockObject(WHITE_BRUSH));
		Ellipse(hdcMemMask, 0, 0, cxBitmap, cyBitmap);

		BitBlt(hdcMemImag, 0, 0, cxBitmap, cyBitmap, hdcMemMask, 0, 0, SRCAND);

		DeleteDC(hdcMemImag);
		DeleteDC(hdcMemMask);

		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hdcMemImag = CreateCompatibleDC(hdc);
		SelectObject(hdcMemImag, hBitmapImag);
		hdcMemMask = CreateCompatibleDC(hdc);
		SelectObject(hdcMemMask, hBitmapMask);

		x = (cxClient - cxBitmap) / 2;
		y = (cyClient - cyBitmap) / 2;

		BitBlt(hdc, 0, 0, cxBitmap, cyBitmap, hdcMemMask, 0, 0, 0x220326);
		BitBlt(hdc, 0, 0, cxBitmap, cyBitmap, hdcMemImag, 0, 0, SRCPAINT);

		DeleteDC(hdcMemImag);
		DeleteDC(hdcMemMask);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteObject(hBitmapImag);
		DeleteObject(hBitmapMask);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
