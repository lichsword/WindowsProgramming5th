#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>
#include "resource.h"

#define OUTWIDTH 6
#define LASTCHAR 127

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("Justify2");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
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
	wndClass.lpszMenuName = szAppName;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Justified Type #2"),
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

void DrawRuler(HDC hdc, RECT* prc)
{
	static int iRuleSize[16] = {360, 72, 144, 72, 216, 72, 144, 72,
		288, 72, 144, 72, 216, 72, 144, 72};
	int i, j;
	POINT ptClient;

	SaveDC(hdc);

	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, 1440, 1440, NULL);
	SetViewportExtEx(hdc, GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY), NULL);

	SetWindowOrgEx(hdc, -720, -720, NULL);

	ptClient.x = prc->right;
	ptClient.y = prc->bottom;
	DPtoLP(hdc, &ptClient, 1);
	ptClient.x -= 360;

	MoveToEx(hdc, 0, -360, NULL);
	LineTo(hdc, ptClient.x, -360);
	MoveToEx(hdc, -360, 0, NULL);
	LineTo(hdc, -360, ptClient.y);

	for (i = 0, j = 0; i <= ptClient.x; i += 1440 / 16, j++)
	{
		MoveToEx(hdc, i, -360, NULL);
		LineTo(hdc, i, -360 - iRuleSize[j % 16]);
	}

	for (i = 0, j = 0; i <= ptClient.y; i += 1440 / 16, j++)
	{
		MoveToEx(hdc, -360, i, NULL);
		LineTo(hdc, -360 - iRuleSize[j % 16], i);
	}

	RestoreDC(hdc, -1);
}

UINT GetCharDesignWidths(HDC hdc, UINT uFirst, UINT uLast, int* piWidths)
{
	HFONT hFont, hFontDesign;
	LOGFONT lf;
	OUTLINETEXTMETRIC otm;

	hFont = GetCurrentObject(hdc, OBJ_FONT);
	GetObject(hFont, sizeof(LOGFONT), &lf);

	otm.otmSize = sizeof(OUTLINETEXTMETRIC);
	GetOutlineTextMetrics(hdc, sizeof(OUTLINETEXTMETRIC), &otm);

	lf.lfHeight = -(int)otm.otmEMSquare;
	lf.lfWidth = 0;
	hFontDesign = CreateFontIndirect(&lf);

	SaveDC(hdc);
	SetMapMode(hdc, MM_TEXT);
	SelectObject(hdc, hFontDesign);

	GetCharWidth(hdc, uFirst, uLast, piWidths);
	SelectObject(hdc, hFont);
	RestoreDC(hdc, -1);

	DeleteObject(hFontDesign);

	return otm.otmEMSquare;
}

void GetScaleWidths(HDC hdc, double* pdWidths)
{
	double dScale;
	HFONT hFont;
	int aiDesignWidths[LASTCHAR + 1];
	int i;
	LOGFONT lf;
	UINT uEMSquare;

	uEMSquare = GetCharDesignWidths(hdc, 0, LASTCHAR, aiDesignWidths);

	hFont = GetCurrentObject(hdc, OBJ_FONT);
	GetObject(hFont, sizeof(LOGFONT), &lf);

	dScale = (double)-lf.lfHeight / (double)uEMSquare;
	for (i = 0; i <= LASTCHAR; i++)
	{
		pdWidths[i] = dScale * aiDesignWidths[i];
	}
}

double GetTextExtentFloat(double* pdWidths, PTSTR psText, int iCount)
{
	double dWidth = 0;
	int i;

	for (i = 0; i < iCount; i++)
	{
		dWidth += pdWidths[psText[i]];
	}

	return dWidth;
}

void Justify(HDC hdc, PTSTR pText, RECT* prc, int iAllign)
{
	double dWidth, adWidths[LASTCHAR + 1];
	int xStart, yStart, cSpaceChars;
	PTSTR pBegin, pEnd;
	SIZE size;

	GetScaleWidths(hdc, adWidths);

	yStart = prc->top;
	do 
	{
		cSpaceChars = 0;

		while (*pText == _T(' '))
		{
			pText++;
		}
		pBegin = pText;

		do 
		{
			pEnd = pText;
			while (*pText != _T('\0') && *(pText++) != _T(' '))
			{

			}
			if (*pText == _T('\0'))
			{
				break;
			}

			cSpaceChars++;
			dWidth = GetTextExtentFloat(adWidths, pBegin, pText - pBegin - 1);
		}while (dWidth < (prc->right - prc->left));

		cSpaceChars --;

		while (*(pEnd - 1) == _T(' '))
		{
			pEnd--;
			cSpaceChars--;
		}

		if (*pText == _T('\0') || cSpaceChars <= 0)
		{
			pEnd = pText;
		}

		GetTextExtentPoint32(hdc, pBegin, pEnd - pBegin, &size);
		switch (iAllign)
		{
		case IDM_ALIGN_LEFT:
			xStart = prc->left;
			break;
		case IDM_ALIGN_RIGHT:
			xStart = prc->right - size.cx;
			break;
		case IDM_ALIGN_CENTER:
			xStart = (prc->right + prc->left - size.cx) / 2;
			break;
		case IDM_ALIGN_JUSTIFIED:
			if (*pText != _T('\0') && cSpaceChars > 0)
			{
				SetTextJustification(hdc, prc->right - prc->left - size.cx, cSpaceChars);
			}
			xStart = prc->left;
			break;
		}

		TextOut(hdc, xStart, yStart, pBegin, pEnd - pBegin);

		SetTextJustification(hdc, 0, 0);
		yStart += size.cy;
		pText = pEnd;

	} while (*pText && yStart < prc->bottom - size.cy);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static CHOOSEFONT cf;
	static DOCINFO di = {sizeof(DOCINFO), _T("Justify2: Printing")};
	static int iAlign = IDM_ALIGN_LEFT;
	static LOGFONT lf;
	static PRINTDLG pd;
	static TCHAR szText[] = _T("Call me Ishmael. Some years ago - never ")
		_T("mind how long precisely - having little ")
		_T("or no money in my purse, and nothing ")
		_T("particular to interest me on shore, I ")
		_T("thought I would sail about a little and ")
		_T("see the watery part of the world. It is ")
		_T("a way I have of driving off the spleen ")
		_T("and regulating the circulation. Whenever ")
		_T("I find myself growing grim about the ")
		_T("mouth; whenever it is a damp, drizzly ")
		_T("November in my soul; whenever I find ")
		_T("myself involuntarily pausing before ")
		_T("coffin warehouses, and bringing up the ")
		_T("rear of every funeral I meet; and ")
		_T("especially whenever my hypos get such an ")
		_T("upper hand of me, that it requires a ")
		_T("strong moral principle to prevent me ")
		_T("from deliberately stepping into the ")
		_T("street, and methodically knocking ")
		_T("people's hats off - then, I account it ")
		_T("high time to get to sea as soon as I ")
		_T("can. This is my substitute for pistol ")
		_T("and ball. With a philosophical flourish ")
		_T("Cato throws himself upon his sword; I ")
		_T("quietly take to the ship. There is ")
		_T("nothing surprising in this. If they but ")
		_T("knew it, almost all men in their degree, ")
		_T("some time or other, cherish very nearly ")
		_T("the same feelings towards the ocean with ")
		_T("me. ");
	BOOL fSuccess;
	HDC hdc, hdcPrn;
	HMENU hMenu;
	int iSavePointSize;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		lf.lfHeight = -GetDeviceCaps(hdc, LOGPIXELSY) / 6;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		_tcscpy(lf.lfFaceName, _T("Timew New Roman"));
		ReleaseDC(hwnd, hdc);

		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.hDC = NULL;
		cf.lpLogFont = &lf;
		cf.iPointSize = 120;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS | CF_TTONLY;
		cf.rgbColors = 0;
		cf.lCustData = 0;
		cf.lpfnHook = NULL;
		cf.lpTemplateName = NULL;
		cf.hInstance = NULL;
		cf.lpszStyle = NULL;
		cf.nFontType = 0;
		cf.nSizeMin = 0;
		cf.nSizeMax = 0;

		return 0;

	case WM_COMMAND:
		hMenu = GetMenu(hwnd);
		switch (LOWORD(wParam))
		{
		case IDM_FILE_PRINT:
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hwnd;
			pd.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;

			if (!PrintDlg(&pd))
			{
				return 0;
			}
			if (NULL == (hdcPrn = pd.hDC))
			{
				MessageBox(hwnd, _T("Cannot obtain Printer DC"), szAppName, MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}

			rect.left = (GetDeviceCaps(hdcPrn, PHYSICALWIDTH) - GetDeviceCaps(hdcPrn, LOGPIXELSX) * OUTWIDTH) / 2
				- GetDeviceCaps(hdcPrn, PHYSICALOFFSETX);
			rect.right = rect.left + GetDeviceCaps(hdcPrn, LOGPIXELSX) * OUTWIDTH;
			rect.top = GetDeviceCaps(hdcPrn, LOGPIXELSY) - GetDeviceCaps(hdcPrn, PHYSICALOFFSETY);
			rect.right = GetDeviceCaps(hdcPrn, PHYSICALWIDTH) - GetDeviceCaps(hdcPrn, LOGPIXELSX) - GetDeviceCaps(hdcPrn, PHYSICALOFFSETX);
			rect.bottom = GetDeviceCaps(hdcPrn, PHYSICALHEIGHT) - GetDeviceCaps(hdcPrn, LOGPIXELSY) - GetDeviceCaps(hdcPrn, PHYSICALOFFSETY);

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			fSuccess = FALSE;

			if ((StartDoc(hdcPrn, &di) > 0) && (StartPage(hdcPrn) > 0))
			{
				iSavePointSize = lf.lfHeight;
				lf.lfHeight = -(GetDeviceCaps(hdcPrn, LOGPIXELSY) * cf.iPointSize) / 720;

				SelectObject(hdcPrn, CreateFontIndirect(&lf));
				lf.lfHeight = iSavePointSize;

				SetTextColor(hdcPrn, cf.rgbColors);
				Justify(hdcPrn, szText, &rect, iAlign);

				if (EndPage(hdcPrn) > 0)
				{
					fSuccess = TRUE;
					EndDoc(hdcPrn);
				}
			}
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			DeleteDC(hdcPrn);
			if (!fSuccess)
			{
				MessageBox(hwnd, _T("Could not print text"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			}

			return 0;

		case IDM_FONT:
			if (ChooseFont(&cf))
			{
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;

		case IDM_ALIGN_LEFT:
		case IDM_ALIGN_RIGHT:
		case IDM_ALIGN_CENTER:
		case IDM_ALIGN_JUSTIFIED:
			CheckMenuItem(hMenu, iAlign, MF_UNCHECKED);
			iAlign = LOWORD(wParam);
			CheckMenuItem(hMenu, iAlign, MF_CHECKED);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		DrawRuler(hdc, &rect);

		rect.left += GetDeviceCaps(hdc, LOGPIXELSX) / 2;
		rect.top += GetDeviceCaps(hdc, LOGPIXELSY) / 2;
		rect.right -= GetDeviceCaps(hdc, LOGPIXELSX) / 4;
		rect.bottom -= GetDeviceCaps(hdc, LOGPIXELSY) / 4;

		SelectObject(hdc, CreateFontIndirect(&lf));
		SetTextColor(hdc, cf.rgbColors);

		Justify(hdc, szText, &rect, iAlign);

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}