#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>
#include "resource.h"

typedef struct
{
	int iDevice, iMapMode;
	BOOL fMatchAspect;
	BOOL fAdvGraphics;
	LOGFONT lf;
	TEXTMETRIC tm;
	TCHAR szFaceName[LF_FULLFACESIZE];
}DLGPARAMS;

#define BCHARFORM _T("0x%04X")

HWND hDlg;
TCHAR szAppName[] = _T("PickFont");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void SetLogFontFromFields(HWND hDlg, DLGPARAMS* pdp);
void SetFieldsFromTextMetric(HWND hdlg, DLGPARAMS* pdp);
void MySetMapMode(HDC hdc, int iMapMode);

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
		_T("PickFont: Create Logical Font"),
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
		if (NULL == hDlg || !IsDialogMessage(hDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static DLGPARAMS dp;
	static TCHAR szText[] = _T("\x41\x42\x43\x44\x45 ")
		_T("\x61\x62\x63\x64\x65 ")
		_T("\xC0\xC1\xC2\xC3\xC4\xC5 ")
		_T("\xE0\xE1\xE2\xE3\xE4\xE5 ")
		_T("\x0390\x0391\x0392\x0393\x0394\x0395 ")
		_T("\x03B0\x03B1\x03B2\x03B3\x03B4\x03B5 ")
		_T("\x0410\x0411\x0412\x0413\x0414\x0415 ")
		_T("\x0430\x0431\x0432\x0433\x0434\x0435 ")
		_T("\x5000\x5001\x5002\x5003\x5004");
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		dp.iDevice = IDM_DEVICE_SCREEN;

		hDlg = CreateDialogParam(((LPCREATESTRUCT)lParam)->hInstance, szAppName, hwnd, DlgProc, (LPARAM)&dp);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_DEVICE_SCREEN:
		case IDM_DEVICE_PRINTER:
			CheckMenuItem(GetMenu(hwnd), dp.iDevice, MF_UNCHECKED);
			dp.iDevice = LOWORD(wParam);
			CheckMenuItem(GetMenu(hwnd), dp.iDevice, MF_CHECKED);
			SendMessage(hDlg, WM_COMMAND, IDOK, 0);
			return 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SetGraphicsMode(hdc, dp.fAdvGraphics ? GM_ADVANCED : GM_COMPATIBLE);
		MySetMapMode(hdc, dp.iMapMode);
		SetMapperFlags(hdc, dp.fMatchAspect);

		GetClientRect(hDlg, &rect);
		rect.bottom ++;
		DPtoLP(hdc, (PPOINT)&rect, 2);

		SelectObject(hdc, CreateFontIndirect(&dp.lf));
		TextOut(hdc, rect.left, rect.bottom, szText, _tcslen(szText));

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hwnd, &ps);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK DlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static DLGPARAMS* pdp;
	static PRINTDLG pd = {sizeof(PRINTDLG)};
	HDC hdcDevice;
	HFONT hFont;

	switch (message)
	{
	case WM_INITDIALOG:
		pdp = (DLGPARAMS*)lParam;

		SendDlgItemMessage(hdlg, IDC_LF_FACENAME, EM_LIMITTEXT, LF_FACESIZE - 1, 0);
		CheckRadioButton(hdlg, IDC_OUT_DEFAULT, IDC_OUT_OUTLINE, IDC_OUT_DEFAULT);
		CheckRadioButton(hdlg, IDC_DEFAULT_QUALITY, IDC_PROOF_QUALITY, IDC_DEFAULT_QUALITY);
		CheckRadioButton(hdlg, IDC_DEFAULT_PITCH, IDC_VARIABLE_PITCH, IDC_DEFAULT_PITCH);
		CheckRadioButton(hdlg, IDC_FF_DONTCARE, IDC_FF_DECORATIVE, IDC_FF_DONTCARE);
		CheckRadioButton(hdlg, IDC_MM_TEXT, IDC_MM_LOGTWIPS, IDC_MM_TEXT);
		SendMessage(hdlg, WM_COMMAND, IDOK, 0);
		//fall through

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHARSET_HELP:
			MessageBox(hdlg, 
				_T("0 = Ansi\n")
				_T("1 = Default\n")
				_T("2 = Symbol\n")
				_T("128 = Shift JIS (Japanese)\n")
				_T("129 = Hangul (Korean)\n")
				_T("130 = Johab (Korean)\n")
				_T("134 = GB 2312 (Simplified Chinese)\n")
				_T("136 = Chinese Big 5 (Traditional Chinese)\n")
				_T("177 = Hebrew\n")
				_T("178 = Arabic\n")
				_T("161 = Greek\n")
				_T("162 = Turkish\n")
				_T("163 = Vietnamese\n")
				_T("204 = Russian\n")
				_T("222 = Thai\n")
				_T("238 = East European\n")
				_T("255 = OEM"),
				szAppName, MB_OK | MB_ICONINFORMATION);
			return TRUE;

		case IDC_OUT_DEFAULT:
			pdp->lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			return TRUE;
		case IDC_OUT_STRING:
			pdp->lf.lfOutPrecision = OUT_STRING_PRECIS;
			return TRUE;
		case IDC_OUT_CHARACTER:
			pdp->lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
			return TRUE;
		case IDC_OUT_STROKE:
			pdp->lf.lfOutPrecision = OUT_STROKE_PRECIS;
			return TRUE;
		case IDC_OUT_TT:
			pdp->lf.lfOutPrecision = OUT_TT_PRECIS;
			return TRUE;
		case IDC_OUT_DEVICE:
			pdp->lf.lfOutPrecision = OUT_DEVICE_PRECIS;
			return TRUE;
		case IDC_OUT_RASTER:
			pdp->lf.lfOutPrecision = OUT_RASTER_PRECIS;
			return TRUE;
		case IDC_OUT_TT_ONLY:
			pdp->lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			return TRUE;
		case IDC_OUT_OUTLINE:
			pdp->lf.lfOutPrecision = OUT_OUTLINE_PRECIS;
			return TRUE;

		case IDC_DEFAULT_QUALITY:
			pdp->lf.lfQuality = DEFAULT_QUALITY;
			return TRUE;
		case IDC_DRAFT_QUALITY:
			pdp->lf.lfQuality = DRAFT_QUALITY;
			return TRUE;
		case IDC_PROOF_QUALITY:
			pdp->lf.lfQuality = PROOF_QUALITY;
			return TRUE;

		case IDC_DEFAULT_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | DEFAULT_PITCH;
			return TRUE;
		case IDC_FIXED_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | FIXED_PITCH;
			return TRUE;
		case IDC_VARIABLE_PITCH:
			pdp->lf.lfPitchAndFamily = (0xF0 & pdp->lf.lfPitchAndFamily) | VARIABLE_PITCH;
			return TRUE;

		case IDC_FF_DONTCARE:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_DONTCARE;
			return TRUE;
		case IDC_FF_ROMAN:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_ROMAN;
			return TRUE;
		case IDC_FF_SWISS:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_SWISS;
			return TRUE;
		case IDC_FF_MODERN:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_MODERN;
			return TRUE;
		case IDC_FF_SCRIPT:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_SCRIPT;
			return TRUE;
		case IDC_FF_DECORATIVE:
			pdp->lf.lfPitchAndFamily = (0x0F & pdp->lf.lfPitchAndFamily) | FF_DECORATIVE;
			return TRUE;

		case IDC_MM_TEXT:
		case IDC_MM_LOMETRIC:
		case IDC_MM_HIMETRIC:
		case IDC_MM_LOENGLISH:
		case IDC_MM_HIENGLISH:
		case IDC_MM_TWIPS:
		case IDC_MM_LOGTWIPS:
			pdp->iMapMode = LOWORD(wParam);
			return TRUE;

		case IDOK:
			SetLogFontFromFields(hdlg, pdp);

			pdp->fMatchAspect = IsDlgButtonChecked(hdlg, IDC_MATCH_ASPECT);
			pdp->fAdvGraphics = IsDlgButtonChecked(hdlg, IDC_ADV_GRAPHICS);

			if (pdp->iDevice == IDM_DEVICE_SCREEN)
			{
				hdcDevice = CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
			}
			else
			{
				pd.hwndOwner = hdlg;
				pd.Flags = PD_RETURNDEFAULT | PD_RETURNIC;
				pd.hDevNames = NULL;
				pd.hDevMode = NULL;

				PrintDlg(&pd);
				hdcDevice = pd.hDC;
			}

			MySetMapMode(hdcDevice, pdp->iMapMode);
			SetMapperFlags(hdcDevice, pdp->fMatchAspect);

			hFont = CreateFontIndirect(&pdp->lf);
			SelectObject(hdcDevice, hFont);

			GetTextMetrics(hdcDevice, &pdp->tm);
			GetTextFace(hdcDevice, LF_FULLFACESIZE, pdp->szFaceName);
			DeleteDC(hdcDevice);
			DeleteObject(hFont);

			SetFieldsFromTextMetric(hdlg, pdp);
			InvalidateRect(GetParent(hdlg), NULL, TRUE);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void SetLogFontFromFields(HWND hdlg, DLGPARAMS* pdp)
{
	pdp->lf.lfHeight = GetDlgItemInt(hdlg, IDC_LF_HEIGHT, NULL, TRUE);
	pdp->lf.lfWidth = GetDlgItemInt(hdlg, IDC_LF_WIDTH, NULL, TRUE);
	pdp->lf.lfEscapement = GetDlgItemInt(hdlg, IDC_LF_ESCAPE, NULL, TRUE);
	pdp->lf.lfOrientation = GetDlgItemInt(hdlg, IDC_LF_ORIENT, NULL, TRUE);
	pdp->lf.lfWeight = GetDlgItemInt(hdlg, IDC_LF_WEIGHT, NULL, TRUE);
	pdp->lf.lfCharSet = GetDlgItemInt(hdlg, IDC_LF_CHARSET, NULL, TRUE);

	pdp->lf.lfItalic = IsDlgButtonChecked(hdlg, IDC_LF_ITALIC) == BST_CHECKED;
	pdp->lf.lfUnderline = IsDlgButtonChecked(hdlg, IDC_LF_UNDER) == BST_CHECKED;
	pdp->lf.lfStrikeOut = IsDlgButtonChecked(hdlg, IDC_LF_STRIKE) == BST_CHECKED;

	GetDlgItemText(hdlg, IDC_LF_FACENAME, pdp->lf.lfFaceName, LF_FACESIZE);
}

void SetFieldsFromTextMetric(HWND hdlg, DLGPARAMS* pdp)
{
	TCHAR szBuffer[10];
	TCHAR* szYes = _T("Yes");
	TCHAR* szNo = _T("No");
	TCHAR* szFamily[] = {
		_T("Dont't Know"), _T("Roman"), _T("Swiss"), _T("Modern"), _T("Script"), _T("Decorative"), _T("Undefined")
	};
	SetDlgItemInt(hdlg, IDC_TM_HEIGHT, pdp->tm.tmHeight, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_ASCENT, pdp->tm.tmAscent, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DESCENT, pdp->tm.tmDescent, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_INTLEAD, pdp->tm.tmInternalLeading, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_EXTLEAD, pdp->tm.tmExternalLeading, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_AVECHAR, pdp->tm.tmAveCharWidth, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_MAXCHAR, pdp->tm.tmMaxCharWidth, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_WEIGHT, pdp->tm.tmWeight, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_OVERHANG, pdp->tm.tmOverhang, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DIGASPX, pdp->tm.tmDigitizedAspectX, TRUE);
	SetDlgItemInt(hdlg, IDC_TM_DIGASPY, pdp->tm.tmDigitizedAspectY, TRUE);
	
	_stprintf(szBuffer, BCHARFORM, pdp->tm.tmFirstChar);
	SetDlgItemText(hdlg, IDC_TM_FIRSTCHAR, szBuffer);

	_stprintf(szBuffer, BCHARFORM, pdp->tm.tmLastChar);
	SetDlgItemText(hdlg, IDC_TM_LASTCHAR, szBuffer);

	_stprintf(szBuffer, BCHARFORM, pdp->tm.tmDefaultChar);
	SetDlgItemText(hdlg, IDC_TM_DEFCHAR, szBuffer);

	_stprintf(szBuffer, BCHARFORM, pdp->tm.tmBreakChar);
	SetDlgItemText(hdlg, IDC_TM_BREAKCHAR, szBuffer);

	SetDlgItemText(hdlg, IDC_TM_ITALIC, pdp->tm.tmItalic ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_UNDER, pdp->tm.tmUnderlined ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_STRUCK, pdp->tm.tmStruckOut ? szYes : szNo);

	SetDlgItemText(hdlg, IDC_TM_VARIABLE, TMPF_FIXED_PITCH & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_VECTOR, TMPF_VECTOR & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_TRUETYPE, TMPF_TRUETYPE & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_DEVICE, TMPF_DEVICE & pdp->tm.tmPitchAndFamily ? szYes : szNo);
	SetDlgItemText(hdlg, IDC_TM_FAMILY, szFamily[min(6, pdp->tm.tmPitchAndFamily >> 4)]);

	SetDlgItemInt(hdlg, IDC_TM_CHARSET, pdp->tm.tmCharSet, FALSE);
	SetDlgItemText(hdlg, IDC_TM_FACENAME, pdp->szFaceName);
}

void MySetMapMode(HDC hdc, int iMapMode)
{
	switch (iMapMode)
	{
	case IDC_MM_TEXT: 
		SetMapMode(hdc, MM_TEXT);
		break;

	case IDC_MM_LOMETRIC: 
		SetMapMode(hdc, MM_LOMETRIC);
		break;

	case IDC_MM_HIMETRIC: 
		SetMapMode(hdc, MM_HIMETRIC);
		break;

	case IDC_MM_LOENGLISH: 
		SetMapMode(hdc, MM_LOENGLISH);
		break;

	case IDC_MM_HIENGLISH: 
		SetMapMode(hdc, MM_HIENGLISH);
		break;

	case IDC_MM_TWIPS: 
		SetMapMode(hdc, MM_TWIPS);
		break;

	case IDC_MM_LOGTWIPS: 
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 1440, 1440, NULL);
		SetViewportExtEx(hdc, GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY), NULL);
		break;
	}
}