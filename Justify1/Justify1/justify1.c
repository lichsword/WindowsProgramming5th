#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("Justify1");

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
		_T("Justified Type #1"),
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

void Justify(HDC hdc, PTSTR pText, RECT* prc, int iAllign)
{
	int xStart, yStart, cSpaceChars;
	PTSTR pBegin, pEnd;
	SIZE size;

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
			GetTextExtentPoint32(hdc, pBegin, pText - pBegin - 1, &size);
		}while (size.cx < (prc->right - prc->left));

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
	static DOCINFO di = {sizeof(DOCINFO), _T("Justify1: Printing")};
	static int iAlign = IDM_ALIGN_LEFT;
	static LOGFONT lf;
	static PRINTDLG pd;
	static TCHAR szText[] = _T("You don't know about me without you have ")
		_T("read a book by the name of The Adventures of Tom Sawyer; but ")
		_T("that ain't no matter. That book was made by Mr. Mark Twain, and ")
		_T("he told the truth, mainly. There was things which he stretched, ")
		_T("but mainly he told the truth. That is nothing. I never seen anybody ")
		_T("but lied one time or another, without it was Aunt Polly, or the widow, ")
		_T("or maybe Mary. Aunt Polly-Tom's Aunt Polly, she is-and Mary, and the ")
		_T("Widow Douglas is all told about in that book, which is mostly a true book, ")
		_T("with some stretchers, as I said before.");
	BOOL fSuccess;
	HDC hdc, hdcPrn;
	HMENU hMenu;
	int iSavePointSize;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);

		cf.lStructSize = sizeof(CHOOSEFONT);
		cf.hwndOwner = hwnd;
		cf.hDC = NULL;
		cf.lpLogFont = &lf;
		cf.iPointSize = 0;
		cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
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

			rect.left = GetDeviceCaps(hdcPrn, LOGPIXELSX) - GetDeviceCaps(hdcPrn, PHYSICALOFFSETX);
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