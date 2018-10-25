#include<windows.h>
#include<tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("KeyView1");
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
		_T("Keyboard Message Viewer #1"),
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
	static int	cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar;
	static int	cLinesMax, cLines;
	static PMSG	pmsg;
	static RECT	rectScroll;
	static TCHAR szTop[] = _T("Message Key Char Repeat Scan Ext Alt Prev Tran");
	static TCHAR szUnd[] = _T("_______ ___ ____ ______ ____ ___ ___ ____ ____");
	static TCHAR* szFormat[2] = {
		_T("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s"),
		_T("%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s")
	};
	static TCHAR* szYes = _T("Yes");
	static TCHAR* szNo = _T("No");
	static TCHAR* szDown = _T("Down");
	static TCHAR* szUp = _T("Up");

	static TCHAR* szMessage[] = {
		_T("WM_KEYDOWN"),
		_T("WM_KEYUP"),
		_T("WM_CHAR"),
		_T("WM_DEADCHAR"),
		_T("WM_SYSKEYDOWN"),
		_T("WM_SYSKEYUP"),
		_T("WM_SYSCHAR"),
		_T("WM_SYSDEADCHAR")
	};
	HDC				hdc;
	int				i, iType;
	PAINTSTRUCT		ps;
	TCHAR			szBuffer[128], szKeyName[32];
	TEXTMETRIC		tm;

	switch (message)
	{
	case WM_CREATE:
	case WM_DISPLAYCHANGE:
		cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
		cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;
		ReleaseDC(hwnd, hdc);

		if (pmsg)
		{
			free(pmsg);
		}
		cLinesMax = cyClientMax / cyChar;
		pmsg = malloc(cLinesMax * sizeof(MSG));
		cLines = 0;
		
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		rectScroll.left = 0;
		rectScroll.right = cxClient;
		rectScroll.top = cyChar;
		rectScroll.bottom = cyChar * (cyClient / cyChar);

		InvalidateRect(hwnd, NULL, TRUE);

		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		for (i = cLinesMax - 1; i > 0; i--)
		{
			pmsg[i] = pmsg[i - 1];
		}
		pmsg[0].hwnd = hwnd;
		pmsg[0].message = message;
		pmsg[0].wParam = wParam;
		pmsg[0].lParam = lParam;
		cLines = min(cLines + 1, cLinesMax);
		ScrollWindow(hwnd, 0, -cyChar, &rectScroll, &rectScroll);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 0, 0, szTop, _tcslen(szTop));
		TextOut(hdc, 0, 0, szUnd, _tcslen(szUnd));

		for (i = 0; i < min(cLines, cyClient / cyChar - 1); i++)
		{
			iType = pmsg[i].message == WM_CHAR 
				|| pmsg[i].message == WM_SYSCHAR
				|| pmsg[i].message == WM_DEADCHAR
				|| pmsg[i].message == WM_SYSDEADCHAR;
			GetKeyNameText(pmsg[i].lParam, szKeyName, sizeof(szKeyName) / sizeof(TCHAR));
			TextOut(hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer, 
				_stprintf(szBuffer, szFormat[iType], szMessage[pmsg[i].message - WM_KEYFIRST],
				pmsg[i].wParam,
				(iType ? _T(" ") : szKeyName),
				(TCHAR)(iType ? pmsg[i].wParam : _T(' ')),
				LOWORD(pmsg[i].lParam),
				HIWORD(pmsg[i].lParam) & 0xff,
				0x01000000 & pmsg[i].lParam ? szYes : szNo,
				0x20000000 & pmsg[i].lParam ? szYes : szNo,
				0x40000000 & pmsg[i].lParam ? szDown : szUp,
				0x80000000 & pmsg[i].lParam ? szUp : szDown
				));
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}