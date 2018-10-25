#include<windows.h>
#include<tchar.h>

#define DIVISIONS		5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szChildClass[] = _T("Checker3_Child");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Checker3");
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

	wndClass.lpfnWndProc = ChildWndProc;
	wndClass.cbWndExtra = sizeof(long);
	wndClass.hIcon = NULL;
	wndClass.lpszClassName = szChildClass;
	RegisterClass(&wndClass);

	hWnd = CreateWindow(szAppName,
		_T("Checker3 Mouse Hit-Test Demo"),
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
	static HWND hwndChild[DIVISIONS][DIVISIONS];
	int cxBlock, cyBlock, x, y;

	switch (message)
	{
	case WM_CREATE:
		for (x = 0; x < DIVISIONS; x++)
		{
			for (y = 0; y < DIVISIONS; y++)
			{
				hwndChild[x][y] = CreateWindow(szChildClass, NULL, WS_CHILDWINDOW | WS_VISIBLE,
					0, 0, 0, 0,
					hwnd, (HMENU)(y << 8 | x),
					(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
			}
		}
		return 0;

	case WM_SIZE:
		cxBlock = LOWORD(lParam) / DIVISIONS;
		cyBlock = HIWORD(lParam) / DIVISIONS;
		for (x = 0; x < DIVISIONS; x++)
		{
			for (y = 0; y < DIVISIONS; y++)
			{
				MoveWindow(hwndChild[x][y], x * cxBlock, y * cyBlock, cxBlock, cyBlock, TRUE);
			}
		}
		return 0;

	case WM_LBUTTONDOWN:
		MessageBeep(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		SetWindowLong(hwnd, 0, 0);
		return 0;

	case WM_LBUTTONDOWN:
		SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0));
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		Rectangle(hdc, 0, 0, rect.right, rect.bottom);

		if (GetWindowLong(hwnd, 0))
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, rect.right, rect.bottom);
			MoveToEx(hdc, 0, rect.bottom, NULL);
			LineTo(hdc, rect.right, 0);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}