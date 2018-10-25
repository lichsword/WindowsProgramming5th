#include<windows.h>
#include<tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("StockFont");
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
		_T("Stock Fonts"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
	static struct  
	{
		int idStockFont;
		TCHAR* szStockFont;
	}
	stockFont[] = {
		OEM_FIXED_FONT,					_T("OEM_FIXED_FONT"),
		ANSI_FIXED_FONT,				_T("ANSI_FIXED_FONT"),
		ANSI_VAR_FONT,					_T("ANSI_VAR_FONT"),
		SYSTEM_FONT,					_T("SYSTEM_FONT"),
		DEVICE_DEFAULT_FONT,			_T("DEVICE_DEFAULT_FONT"),
		SYSTEM_FIXED_FONT,				_T("SYSTEM_FIXED_FONT"),
		DEFAULT_GUI_FONT,				_T("DEFAULT_GUI_FONT")
	};

	static int				iFont, cFonts = sizeof stockFont / sizeof stockFont[0];
	HDC						hdc;
	int						i, x, y, cxGrid, cyGrid;
	PAINTSTRUCT				ps;
	TCHAR					szFaceName[LF_FACESIZE], szBuffer[LF_FACESIZE + 64];
	TEXTMETRIC				tm;


	switch (message)
	{
	case WM_CREATE:
		SetScrollRange(hwnd, SB_VERT, 0, cFonts - 1, TRUE);
		return 0;
	case WM_DISPLAYCHANGE:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			iFont = 0;
			break;
		case SB_BOTTOM:
			iFont = cFonts - 1;
			break;
		case SB_LINEUP:
		case SB_PAGEUP:
			iFont -= 1;
			break;
		case SB_LINEDOWN:
		case SB_PAGEDOWN:
			iFont += 1;
			break;
		case SB_THUMBPOSITION:
			iFont = HIWORD(wParam);
			break;
		default:
			break;
		}
		iFont = max(0, min(cFonts - 1, iFont));
		SetScrollPos(hwnd, SB_VERT, iFont, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_HOME:
			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
		case VK_LEFT:
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_NEXT:
		case VK_RIGHT:
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		default:
			break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, GetStockObject(stockFont[iFont].idStockFont));
		GetTextFace(hdc, LF_FACESIZE, szFaceName);
		GetTextMetrics(hdc, &tm);
		cxGrid = max(3 * tm.tmAveCharWidth, 2 * tm.tmMaxCharWidth);
		cyGrid = tm.tmHeight + 3;
		TextOut(hdc, 0, 0, szBuffer, 
			_stprintf(szBuffer, _T("%s:Face Name = %s, CharSet = %i"), stockFont[iFont].szStockFont, szFaceName, tm.tmCharSet)
			);
		SetTextAlign(hdc, TA_TOP | TA_CENTER);

		for (i = 0; i < 17; i++)
		{
			MoveToEx(hdc, (i + 2) * cxGrid, 2 * cyGrid, NULL);
			LineTo(hdc, (i + 2) * cxGrid, 19 * cyGrid);

			MoveToEx(hdc, cxGrid, (i + 3) * cyGrid, NULL);
			LineTo(hdc, 18 * cxGrid, (i + 3) * cyGrid);
		}
		
		for (i = 0; i < 16; i++)
		{
			TextOut(hdc, (2 * i + 5) * cxGrid / 2, 2 * cyGrid + 2, szBuffer,
				_stprintf(szBuffer, _T("%X-"), i));
			TextOut(hdc, 3 * cxGrid / 2, (i + 3) * cyGrid + 2, szBuffer,
				_stprintf(szBuffer, _T("%X-"), i));
		}
		for (y = 0; y < 16; y++)
		{
			for (x = 0; x < 16; x++)
			{
				TextOut(hdc, (2 * x + 5) * cxGrid / 2, (y + 3) * cyGrid + 2, szBuffer,
					_stprintf(szBuffer, _T("%c"), 16 * x + y));
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