#include<windows.h>
#include<tchar.h>
#include<math.h>

#define TWO_PI	(2.0 * 3.141593)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int cxClient, cyClient;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Clover");
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
		_T("Draw a Clover"),
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
	static HRGN		hRgnClip;
	static int		cxClient, cyClient;
	double			fAngle, fRadius;
	HCURSOR			hCursor;
	HDC				hdc;
	HRGN			hRgnTemp[6];
	int				i;
	PAINTSTRUCT		ps;

	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);

		if (hRgnClip)
		{
			DeleteObject(hRgnClip);
		}
		hRgnTemp[0] = CreateEllipticRgn(0, cyClient / 3, cxClient / 2, 2 * cyClient / 3);
		hRgnTemp[1] = CreateEllipticRgn(cxClient / 2, cyClient / 3, cxClient, 2 * cyClient / 3);
		hRgnTemp[2] = CreateEllipticRgn(cxClient / 3, 0, 2 * cxClient / 3, cyClient / 2);
		hRgnTemp[3] = CreateEllipticRgn(cxClient / 3, cyClient / 2, 2 * cxClient / 3, cyClient);
		hRgnTemp[4] = CreateRectRgn(0, 0, 1, 1);
		hRgnTemp[5] = CreateRectRgn(0, 0, 1, 1);
		hRgnClip = CreateRectRgn(0, 0, 1, 1);

		CombineRgn(hRgnTemp[4], hRgnTemp[0], hRgnTemp[1], RGN_OR);
		CombineRgn(hRgnTemp[5], hRgnTemp[2], hRgnTemp[3], RGN_OR);
		CombineRgn(hRgnClip, hRgnTemp[4], hRgnTemp[5], RGN_XOR);

		for (i = 0; i < 6; i++)
		{
			DeleteObject(hRgnTemp[i]);
		}
		SetCursor(hCursor);
		ShowCursor(FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
		SelectClipRgn(hdc, hRgnClip);
		fRadius = _hypot(cxClient / 2.0, cyClient / 2.0);
		for (fAngle = 0.0; fAngle < TWO_PI; fAngle += TWO_PI / 360)
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, (int)(fRadius * cos(fAngle) + 0.5), (int)(-fRadius * sin(fAngle) + 0.5));
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hRgnClip);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void DrawRectangle(HWND hwnd)
{
	HBRUSH	hBrush;
	HDC		hdc;
	RECT	rect;

	if (cxClient == 0 || cyClient == 0)
	{
		return ;
	}

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush = SelectObject(hdc, WHITE_BRUSH);
	FillRect(hdc, &rect, NULL);

	SetRect(&rect, rand() % cxClient, rand() % cyClient, rand() % cxClient, rand() % cyClient);
	hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
	
	FillRect(hdc, &rect, hBrush);
	ReleaseDC(hwnd, hdc);
	DeleteObject(hBrush);
}