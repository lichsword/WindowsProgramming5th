#include<windows.h>
#include<tchar.h>
#include<math.h>

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Bounce");
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
		_T("Bouncing Ball"),
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
	static HBITMAP hBitmap;
	static int cxClient, cyClient, xCenter, yCenter, cxTotal, cyTotal,
		cxRadius, cyRadius, cxMove, cyMove, xPixel, yPixel;
	HBRUSH hBrush;
	HDC hdc, hdcMem;
	int iScale;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		xPixel = GetDeviceCaps(hdc, ASPECTX);
		yPixel = GetDeviceCaps(hdc, ASPECTY);
		ReleaseDC(hwnd, hdc);

		SetTimer(hwnd, ID_TIMER, 50, NULL);
		return 0;

	case WM_SIZE:
		xCenter = (cxClient = LOWORD(lParam)) / 2;
		yCenter = (cyClient = HIWORD(lParam)) / 2;

		iScale = min(cxClient * xPixel, cyClient * yPixel) / 16;

		cxRadius = iScale / xPixel;
		cyRadius = iScale / yPixel;

		cxMove = max(1, cxRadius / 2);
		cyMove = max(1, cyRadius / 2);

		cxTotal = 2 * (cxRadius + cxMove);
		cyTotal = 2 * (cyRadius + cyMove);

		if (hBitmap)
		{
			DeleteObject(hBitmap);
		}

		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, cxTotal, cyTotal);
		ReleaseDC(hwnd, hdc);

		SelectObject(hdcMem, hBitmap);
		Rectangle(hdcMem, -1, -1, cxTotal + 1, cyTotal + 1);

		hBrush = CreateHatchBrush(HS_DIAGCROSS, 0);
		SelectObject(hdcMem, hBrush);
		SetBkColor(hdcMem, RGB(255, 0, 255));
		Ellipse(hdcMem, cxMove, cyMove, cxTotal - cxMove, cyTotal - cyMove);
		DeleteDC(hdcMem);
		DeleteObject(hBrush);

		return 0;

	case WM_TIMER:
		if (NULL == hBitmap)
		{
			break;
		}

		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);

		BitBlt(hdc, xCenter - cxTotal / 2, yCenter - cyTotal / 2, cxTotal, cyTotal, hdcMem, 0, 0, SRCCOPY);

		ReleaseDC(hwnd, hdc);
		DeleteDC(hdcMem);

		xCenter += cxMove;
		yCenter += cyMove;

		if ((xCenter + cxRadius >= cxClient) || (xCenter - cxRadius <= 0))
		{
			cxMove = -cxMove;
		}
		if ((yCenter + cyRadius >= cyClient) || (yCenter - cyRadius <= 0))
		{
			cyMove = -cyMove;
		}

		return 0;

	case WM_DESTROY:
		if (NULL != hBitmap)
		{
			DeleteObject(hBitmap);
		}
		KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
