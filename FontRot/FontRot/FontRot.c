#include<windows.h>
#include<tchar.h>
#include<math.h>

#include "resource.h"

HFONT EzCreateFont(HDC hdc, TCHAR* szFaceName, int iDeciPtHeight, int iDeciPtWidth, int iAttributes, BOOL fLogRes);

#define EZ_ATTR_BOLD			1
#define EZ_ATTR_ITALIC			2
#define EZ_ATTR_UNDERLINE		4
#define EZ_ATTR_STRIKEOUT		8

HFONT EzCreateFont(HDC hdc, TCHAR* szFaceName, int iDeciPtHeight, int iDeciPtWidth, int iAttributes, BOOL fLogRes)
{
	FLOAT cxDpi, cyDpi;
	HFONT hFont;
	LOGFONT lf;
	POINT pt;
	TEXTMETRIC tm;

	SaveDC(hdc);

	SetGraphicsMode(hdc, GM_ADVANCED);
	ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 0, 0, NULL);

	if (fLogRes)
	{
		cxDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSX);
		cyDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSY);
	}
	else
	{
		cxDpi = (FLOAT)(25.4 * GetDeviceCaps(hdc, HORZRES) / GetDeviceCaps(hdc, HORZSIZE));
		cyDpi = (FLOAT)(25.4 * GetDeviceCaps(hdc, VERTRES) / GetDeviceCaps(hdc, VERTSIZE));
	}

	pt.x = (int)(iDeciPtWidth * cxDpi / 72);
	pt.y = (int)(iDeciPtHeight * cyDpi / 72);

	DPtoLP(hdc, &pt, 1);

	lf.lfHeight = -(int)(fabs(pt.y) / 10.0 + 0.5);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = iAttributes & EZ_ATTR_BOLD ? 700 : 0;
	lf.lfItalic = iAttributes & EZ_ATTR_ITALIC ? 1 : 0;
	lf.lfUnderline = iAttributes & EZ_ATTR_UNDERLINE ? 1 : 0;
	lf.lfStrikeOut = iAttributes & EZ_ATTR_STRIKEOUT ? 1 : 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	_tcscpy(lf.lfFaceName, szFaceName);

	hFont = CreateFontIndirect(&lf);
	if (0 != iDeciPtWidth)
	{
		hFont = (HFONT)SelectObject(hdc, hFont);
		GetTextMetrics(hdc, &tm);
		DeleteObject(SelectObject(hdc, hFont));
		lf.lfWidth = (int)(tm.tmAveCharWidth * fabs(pt.x) / fabs(pt.y) + 0.5);

		hFont = CreateFontIndirect(&lf);
	}

	RestoreDC(hdc, -1);
	return hFont;
}

TCHAR szAppName[] = _T("FontRot");
TCHAR szTitle[] = _T("FontRot: Rotated Fonts");

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	static TCHAR szString[] = _T("Rotation");
	HFONT hFont;
	int i;
	LOGFONT lf;

	hFont = EzCreateFont(hdc, _T("Times New Roman"), 540, 0, 0, TRUE);
	GetObject(hFont, sizeof(LOGFONT), &lf);
	DeleteObject(hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextAlign(hdc, TA_BASELINE);
	SetViewportOrgEx(hdc, cxArea / 2, cyArea / 2, NULL);

	for (i = 0; i < 12; i++)
	{
		lf.lfEscapement = lf.lfOrientation = i * 300;
		SelectObject(hdc, CreateFontIndirect(&lf));
		TextOut(hdc, 0, 0, szString, _tcslen(szString));
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
	}
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szResource[] = _T("FontDemo");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

	hInst = hInstance;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = szResource;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		szTitle,
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
	static DOCINFO di = {sizeof(DOCINFO), _T("Font Demo: Printing")};
	static int cxClient, cyClient;
	static PRINTDLG pd = {sizeof(PRINTDLG)};
	BOOL fSuccess;
	HDC hdc, hdcPrn;
	int cxPage, cyPage;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_PRINT:
			pd.hwndOwner = hwnd;
			pd.Flags = PD_RETURNIC | PD_NOPAGENUMS | PD_NOSELECTION;

			if (!PrintDlg(&pd))
			{
				return 0;
			}

			if (NULL == (hdcPrn = pd.hDC))
			{
				MessageBox(hwnd, _T("Cannot obtain Printer DC"), szAppName, MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}

			cxPage = GetDeviceCaps(hdcPrn, HORZRES);
			cyPage = GetDeviceCaps(hdcPrn, VERTRES);

			fSuccess = FALSE;

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);

			if ((StartDoc(hdcPrn, &di) > 0) && (StartPage(hdcPrn) > 0))
			{
				PaintRoutine(hwnd, hdcPrn, cxPage, cyPage);

				if (EndPage(hdcPrn) > 0)
				{
					fSuccess = TRUE;
					EndDoc(hdcPrn);
				}
			}
			DeleteDC(hdcPrn);

			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			if (!fSuccess)
			{
				MessageBox(hwnd, _T("Error encountered during printing"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			}
			return 0;

		case IDM_ABOUT:
			MessageBox(hwnd, _T("Font Demonstration Program\n (c) Charles Petzold, 1998"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
		break;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		PaintRoutine(hwnd, hdc, cxClient, cyClient);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}