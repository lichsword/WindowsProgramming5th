#include<windows.h>
#include<tchar.h>
#include<process.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd;
int cxClient, cyClient;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("RndRctMT");
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

	hwnd = CreateWindow(szAppName,
		_T("Random Rectangles"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

VOID Thread(PVOID pvoid)
{
	HBRUSH hBrush;
	HDC hdc;
	int xLeft, xRight, yTop, yBottom, iRed, iGreen, iBlue;

	while (TRUE)
	{
		if (0 != cxClient || 0 != cyClient)
		{
			xLeft = rand() % cxClient;
			xRight = rand() % cxClient;
			yTop = rand() % cyClient;
			yBottom = rand() % cyClient;
			iRed = rand() % 255;
			iGreen = rand() % 255;
			iBlue = rand() % 255;

			hdc = GetDC(hwnd);
			hBrush = CreateSolidBrush(RGB(iRed, iGreen, iBlue));
			SelectObject(hdc, hBrush);
			Rectangle(hdc, min(xLeft, xRight), min(yTop, yBottom), max(xLeft, xRight), max(yTop, yBottom));

			ReleaseDC(hwnd, hdc);
			DeleteObject(hBrush);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		_beginthread(Thread, 0, NULL);
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}