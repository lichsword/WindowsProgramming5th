#include<windows.h>
#include<tchar.h>

#define DIVISIONS		5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Checker2");
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
		_T("Checker2 Mouse Hit-Test Demo"),
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
	static BOOL fState[DIVISIONS][DIVISIONS];
	static int	cxBlock, cyBlock;
	HDC			hdc;
	static int			x = -1, y = -1;
	PAINTSTRUCT	ps;
	RECT		rect;
	POINT		point;

	switch (message)
	{
	case WM_SIZE:
		cxBlock = LOWORD(lParam) / DIVISIONS;
		cyBlock = HIWORD(lParam) / DIVISIONS;
		return 0;

	case WM_SETFOCUS:
		ShowCursor(TRUE);
		return 0;

	case WM_KILLFOCUS:
		ShowCursor(FALSE);
		return 0;
	case WM_KEYDOWN:
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);

		x = max(0, min(DIVISIONS - 1, point.x / cxBlock));
		y = max(0, min(DIVISIONS - 1, point.y / cyBlock));

		switch (wParam)
		{
		case VK_UP:
			y--;
			break;
		case VK_DOWN:
			y++;
			break;
		case VK_LEFT:
			x--;
			break;
		case VK_RIGHT:
			x++;
			break;
		case VK_HOME:
			x = y = 0;
			break;
		case VK_END:
			x = y = DIVISIONS - 1;
			break;
		case VK_RETURN:
		case VK_SPACE:
			SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x * cxBlock, y * cyBlock));
			break;
		}
		x = (x + DIVISIONS) % DIVISIONS;
		y = (y + DIVISIONS) % DIVISIONS;
		point.x = x * cxBlock + cxBlock / 2;
		point.y = y * cyBlock + cyBlock / 2;

		ClientToScreen(hwnd, &point);
		SetCursorPos(point.x, point.y);
		return 0;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam) / cxBlock;
		y = HIWORD(lParam) / cyBlock;
		if (x < DIVISIONS && y < DIVISIONS)
		{
			fState[x][y] ^= 1;
			rect.left = x * cxBlock;
			rect.right = (x + 1) * cxBlock;
			rect.top = y * cyBlock;
			rect.bottom = (y + 1) * cyBlock;

			InvalidateRect(hwnd, &rect, FALSE);
		}
		else
		{
			MessageBeep(0);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		for (x = 0; x < DIVISIONS; x++)
		{
			for (y = 0; y < DIVISIONS; y++)
			{
				Rectangle(hdc, x * cxBlock, y * cyBlock, (x + 1) * cxBlock, (y + 1) * cyBlock);
				if (fState[x][y])
				{
					MoveToEx(hdc, x * cxBlock, y * cyBlock, NULL);
					LineTo(hdc, (x + 1) * cxBlock, (y + 1) * cyBlock);
					MoveToEx(hdc, x * cxBlock, (y + 1) * cyBlock, NULL);
					LineTo(hdc, (x + 1) * cxBlock, y * cyBlock);
				}
			}
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}