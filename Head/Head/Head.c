#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ListProc(HWND, UINT, WPARAM, LPARAM);

#define ID_LIST		1
#define ID_TEXT		2
#define MAXREAD		8192
#define DIRATTR		(DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | DDL_DIRECTORY | DDL_ARCHIVE | DDL_DRIVES)
#define DTFLAGS		(DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

WNDPROC OldList;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	TCHAR szAppName[] = _T("head");

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("head"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
	static BOOL			bValidFile;
	static TCHAR		buffer[MAXREAD];
	static HWND			hwndList, hwndText;
	static RECT			rect;
	static TCHAR		szfile[MAX_PATH + 1];
	HANDLE				hFile;
	HDC					hdc;
	int					i, cxChar, cyChar;
	PAINTSTRUCT			ps;
	TCHAR				szBuffer[MAX_PATH + 1];

	switch (message)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		rect.left = 20 * cxChar;
		rect.top = 3 * cyChar;

		hwndList = CreateWindow(_T("listbox"), NULL,
			WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
			cxChar, cyChar * 3, cxChar * 13 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 10,
			hwnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		GetCurrentDirectory(MAX_PATH + 1, szBuffer);

		hwndText = CreateWindow(_T("static"), szBuffer, WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT, 
			cxChar, cyChar, cxChar * MAX_PATH, cyChar,
			hwnd, (HMENU)ID_TEXT, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		OldList = (WNDPROC)SetWindowLong(hwndList, GWL_WNDPROC, (LPARAM)ListProc);
		SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)_T("*.*"));
		return 0;

	case WM_SIZE:
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_DBLCLK)
		{
			if (LB_ERR == (i = SendMessage(hwndList, LB_GETCURSEL, 0, 0)))
			{
				break;
			}

			SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)szBuffer);
			if (INVALID_HANDLE_VALUE != (hFile = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
			{
				CloseHandle(hFile);
				bValidFile = TRUE;
				_tcscpy(szfile, szBuffer);
				GetCurrentDirectory(MAX_PATH + 1, szBuffer);

				if (szBuffer[_tcslen(szBuffer) - 1] != '\\')
				{
					_tcscat(szBuffer, _T("\\"));
				}
				SetWindowText(hwndText, _tcscat(szBuffer, szfile));
			}
			else
			{
				bValidFile = FALSE;
				szBuffer[_tcslen(szBuffer) - 1] = '\0';

				if (!SetCurrentDirectory(szBuffer + 1))
				{
					szBuffer[3] = _T(':');
					szBuffer[4] = _T('\0');
					SetCurrentDirectory(szBuffer + 2);
				}
				GetCurrentDirectory(MAX_PATH + 1, szBuffer);
				SetWindowText(hwndText, szBuffer);
				SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
				SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)_T("*.*"));
			}

			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		if (!bValidFile)
		{
			break;
		}
		if (INVALID_HANDLE_VALUE == (hFile = CreateFile(szfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
		{
			bValidFile = FALSE;
			break;
		}
		ReadFile(hFile, buffer, MAXREAD, &i, NULL);
		CloseHandle(hFile);
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

		DrawText(hdc, buffer, i, &rect, DTFLAGS);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ListProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_RETURN)
	{
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(1, LBN_DBLCLK), (LPARAM)hwnd);
	}

	return CallWindowProc(OldList, hwnd, message, wParam, lParam);
}