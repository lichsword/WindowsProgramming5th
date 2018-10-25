#include<windows.h>
#include<tchar.h>
#include<math.h>

struct  
{
	int		iStyle;
	TCHAR*	szText;
}button[] = 
{
	BS_PUSHBUTTON,			_T("PUSHBUTTON"),
	BS_DEFPUSHBUTTON,		_T("DEFPUSHBUTTON"),
	BS_CHECKBOX,			_T("CHECKBOX"),
	BS_AUTOCHECKBOX,		_T("AUTOCHECKBOX"),
	BS_RADIOBUTTON,			_T("RADIOBUTTON"),
	BS_3STATE,				_T("3STATE"),
	BS_AUTO3STATE,			_T("AUTO3STATE"),
	BS_GROUPBOX,			_T("GROUPBOX"),
	BS_AUTORADIOBUTTON,		_T("AUTORADIOBUTTON"),
	BS_OWNERDRAW,			_T("OWNERDRAW")
};

#define NUM		(sizeof(button) / sizeof(button[0]))

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("BtnLook");
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
		_T("Button Look"),
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
	static HWND		hwndButton[NUM];
	static RECT		rect;
	static TCHAR	szTop[] =		_T("message        wParam       lParam"),
					szUnd[] =       _T("_______        ______       ______"),
					szFormat[] =    _T("%-16s%04X-%04X %04X-%04X"),
					szBuffer[50];
	static int		cxChar, cyChar;
	HDC				hdc;
	PAINTSTRUCT		ps;
	int				i;

	switch (message)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		for (i = 0; i < NUM; i++)
		{
			hwndButton[i] = CreateWindow(_T("button"), 
				button[i].szText, 
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				cxChar, cyChar * (1 + 2 * i),
				20 * cxChar, 7 * cyChar / 4,
				hwnd, (HMENU)i,
				((LPCREATESTRUCT)lParam)->hInstance, NULL);
		}
		return 0;

	case WM_SIZE:
		rect.left = 24 * cxChar;
		rect.top = 2 * cyChar;
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 24 * cxChar, cyChar, szTop, _tcslen(szTop));
		TextOut(hdc, 24 * cxChar, cyChar, szUnd, _tcslen(szUnd));

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DRAWITEM:
	case WM_COMMAND:
		ScrollWindow(hwnd, 0, -cyChar, &rect, &rect);

		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		TextOut(hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
			szBuffer,
			_stprintf(szBuffer, szFormat, message == WM_DRAWITEM ? _T("WM_DRAWITEM") : _T("WM_COMMAND"),
						HIWORD(wParam), LOWORD(wParam), HIWORD(lParam), LOWORD(lParam)));
		ReleaseDC(hwnd, hdc);
		ValidateRect(hwnd, &rect);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}