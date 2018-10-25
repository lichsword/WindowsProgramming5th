#include<windows.h>
#include<tchar.h>

#define MAXPOINTS		1000

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Connect");
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
		_T("Connect-the-points Mouse Demo"),
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
	static POINT	pt[MAXPOINTS];
	static int		iCount;
	HDC				hdc;
	int				i, j;
	PAINTSTRUCT		ps;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		iCount = 0;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON && iCount < 1000)
		{
			pt[iCount].x = LOWORD(lParam);
			pt[iCount++].y = HIWORD(lParam);
			hdc = GetDC(hwnd);
			SetPixel(hdc, LOWORD(lParam), HIWORD(lParam), 0);
			ReleaseDC(hwnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);
		for (i = 0; i < iCount - 1; i++)
		{
			for (j = i + 1; j < iCount; j++)
			{
				MoveToEx(hdc, pt[i].x, pt[i].y, NULL);
				LineTo(hdc, pt[j].x, pt[j].y);
			}
			Sleep(100);
		}

		ShowCursor(FALSE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}