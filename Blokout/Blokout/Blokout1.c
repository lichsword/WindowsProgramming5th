#include<windows.h>
#include<tchar.h>

#define DIVISIONS		5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("BlokOut1");
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
		_T("Mouse Button Demo"),
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

void DrawBoxOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)
{
	HDC hdc;
	hdc = GetDC(hwnd);
	SetROP2(hdc, R2_NOT);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
	ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL fBlocking, fValidBox;
	static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
	HDC			hdc;
	PAINTSTRUCT	ps;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		ptBeg.x = ptEnd.x = LOWORD(lParam);
		ptBeg.y = ptEnd.y = HIWORD(lParam);
		DrawBoxOutline(hwnd, ptBeg, ptEnd);
		SetCapture(hwnd);
		SetCursor(LoadCursor(NULL, IDC_CROSS));
		fBlocking = TRUE;
		return 0;

	case WM_MOUSEMOVE:
		if (fBlocking)
		{
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ptEnd.x = LOWORD(lParam);
			ptEnd.y = HIWORD(lParam);
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
		}
		return 0;
	case WM_LBUTTONUP:
		if (fBlocking)
		{
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ptBoxBeg = ptBeg;
			ptBoxEnd.x = LOWORD(lParam);
			ptBoxEnd.y = HIWORD(lParam);

			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			fBlocking = FALSE;
			fValidBox = TRUE;

			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;

	case WM_CHAR:
		if (fBlocking & (wParam == VK_ESCAPE))
		{
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			fBlocking = FALSE;
		}
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		if (fValidBox)
		{
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
		}
		if (fBlocking)
		{
			SetROP2(hdc, R2_NOT);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			Rectangle(hdc, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}