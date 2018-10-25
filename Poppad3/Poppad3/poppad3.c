#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<commdlg.h>
#include "resource.h"

#define EDITID		1
#define UNTITLED	_T("(untitled)")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

void PopFileInitialize	(HWND);
BOOL PopFileOpenDlg		(HWND, PTSTR, PTSTR);
BOOL PopFileSaveDlg		(HWND, PTSTR, PTSTR);
BOOL PopFileRead		(HWND, PTSTR);
BOOL PopFileWrite		(HWND, PTSTR);

HWND PopFindFindDlg		(HWND);
HWND PopFindReplaceDlg	(HWND);
BOOL PopFindFindText	(HWND, int*, LPFINDREPLACE);
BOOL PopFindReplaceText	(HWND, int*, LPFINDREPLACE);
BOOL PopFindNextText	(HWND, int*);
BOOL PopFindValidFind	(void);

void PopFontInitialize	(HWND);
BOOL PopFontChooseFont	(HWND);
void PopFontSetFont		(HWND);
void PopFontDeinitialize(void);

BOOL PopPrntPrintFile	(HINSTANCE, HWND, HWND, PTSTR);

static HWND		hDlgModeLess;
static TCHAR	szAppName[] = _T("PopPad3");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	HACCEL	 hAccel;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, szAppName);
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
		NULL,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		szCmdLine);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	hAccel = LoadAccelerators(hInstance, szAppName);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hDlgModeLess == NULL || !IsDialogMessage(hDlgModeLess, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void DoCaption(HWND hwnd, TCHAR* szTitleName)
{
	TCHAR szCaption[64 + MAX_PATH];
	_stprintf(szCaption, _T("%s - %s"), szAppName, szTitleName[0] ? szTitleName : UNTITLED);
	SetWindowText(hwnd, szCaption);
}
void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName)
{
	TCHAR szBuffer[64 + MAX_PATH];
	_stprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED);
	MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}
short AskAboutSave(HWND hwnd, TCHAR* szTitleName)
{
	TCHAR	szBuffer[64 + MAX_PATH];
	int		iReturn;

	_stprintf(szBuffer, _T("Save current changes in %s?"), szTitleName[0] ? szTitleName : UNTITLED);
	iReturn = MessageBox(hwnd, szBuffer, szAppName, MB_YESNOCANCEL | MB_ICONQUESTION);
	if (IDYES == iReturn)
	{
		if (!SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
		{
			iReturn = IDCANCEL;
		}
	}

	return iReturn;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static	BOOL		bNeedSave = FALSE;
	static  HINSTANCE	hInst;
	static	HWND		hwndEdit;
	static	int			iOffset;
	static	TCHAR		szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static  UINT		messageFindReplace;
	int					iSelBeg, iSelEnd, iEnable;
	LPFINDREPLACE		pfr;

	switch (message)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;
		hwndEdit = CreateWindow(_T("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0, hwnd, (HMENU)EDITID, hInst, NULL);
		SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0);

		PopFileInitialize(hwnd);
		PopFontInitialize(hwndEdit);

		messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

		DoCaption(hwnd, szTitleName);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_INITMENUPOPUP:
		switch (lParam)
		{
		case 1:
			EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO, 
				SendMessage(hwndEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, 
				IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);

			SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&iSelBeg, (LPARAM)&iSelEnd);
			iEnable = (iSelBeg != iSelEnd)? MF_ENABLED : MF_GRAYED;
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, iEnable);
			break;

		case 2:
			iEnable = (hDlgModeLess == NULL ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_FIND,		iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_NEXT,		iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_REPLACE,	iEnable);
			break;
		}
		return 0;

	case WM_COMMAND:
		if (0 != lParam && LOWORD(wParam) == EDITID)
		{
			switch (HIWORD(wParam))
			{
			case EN_UPDATE:
				bNeedSave = TRUE;
				return 0;

			case EN_ERRSPACE:
			case EN_MAXTEXT:
				MessageBox(hwnd, _T("Edit control out of space."), szAppName, MB_OK | MB_ICONSTOP);
				return 0;
			}
			break;
		}

		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
			if (bNeedSave && IDCANCEL == AskAboutSave(hwnd, szTitleName))
			{
				return 0;
			}
			
			SetWindowText(hwndEdit, _T("\0"));
			szFileName[0] = '\0';
			szTitleName[0] = '\0';
			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			return 0;
			
		case IDM_FILE_OPEN:
			if (bNeedSave && IDCANCEL == AskAboutSave(hwnd, szTitleName))
			{
				return 0;
			}
			if (PopFileOpenDlg(hwnd, szFileName, szTitleName))
			{
				if (!PopFileRead(hwndEdit, szFileName))
				{
					OkMessage(hwnd, _T("Could not read file %s!"), szTitleName);
					szFileName[0] = '\0';
					szTitleName[0] = '\0';
				}
			}

			DoCaption(hwnd, szTitleName);
			bNeedSave = FALSE;
			return 0;

		case IDM_FILE_SAVE:
			if (szFileName[0])
			{
				if (PopFileWrite(hwndEdit, szFileName))
				{
					bNeedSave = FALSE;
					return 1;
				}
				else
				{
					OkMessage(hwnd, _T("Could not write file%s"), szTitleName);
					return 0;
				}
			}
		case IDM_FILE_SAVE_AS:
			if (PopFileSaveDlg(hwnd, szFileName, szTitleName))
			{
				DoCaption(hwnd, szTitleName);
				if (PopFileWrite(hwndEdit, szFileName))
				{
					bNeedSave = FALSE;
					return 1;
				}
				else
				{
					OkMessage(hwnd, _T("Could not write file %s"), szTitleName);
					return 0;
				}
			}
			return 0;

		case IDM_FILE_PRINT:
			if (!PopPrntPrintFile(hInst, hwnd, hwndEdit, szTitleName))
			{
				OkMessage(hwnd, _T("Could not print file %s"), szTitleName);
			}
			return 0;

		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		case IDM_EDIT_UNDO:
			SendMessage(hwndEdit, WM_UNDO, 0, 0);
			return 0;

		case IDM_EDIT_CUT:
			SendMessage(hwndEdit, WM_CUT, 0, 0);
			return 0;

		case IDM_EDIT_COPY:
			SendMessage(hwndEdit, WM_COPY, 0, 0);
			return 0;

		case IDM_EDIT_PASTE:
			SendMessage(hwndEdit, WM_PASTE, 0, 0);
			return 0;

		case IDM_EDIT_CLEAR:
			SendMessage(hwndEdit, WM_CLEAR, 0, 0);
			return 0;

		case IDM_EDIT_SELECT_ALL:
			SendMessage(hwndEdit, EM_SETSEL, 0, -1);
			return 0;

		case IDM_SEARCH_FIND:
			SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
			hDlgModeLess = PopFindFindDlg(hwnd);
			return 0;

		case IDM_SEARCH_NEXT:
			SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
			if (PopFindValidFind())
			{
				PopFindNextText(hwndEdit, &iOffset);
			}
			else
			{
				hDlgModeLess = PopFindFindDlg(hwnd);
			}
			return 0;

		case IDM_SEARCH_REPLACE:
			SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
			hDlgModeLess = PopFindReplaceDlg(hwnd);
			return 0;

		case IDM_FORMAT_FONT:
			if (PopFontChooseFont(hwnd))
			{
				PopFontSetFont(hwndEdit);
			}
			return 0;

		case IDM_HELP:
			OkMessage(hwnd, _T("Help not yet implemented!"), _T("\0"));
			return 0;

		case IDM_APP_ABOUT:
			DialogBox(hInst, _T("AboutBox"), hwnd, AboutDlgProc);
			return 0;
		}
		break;

	case WM_CLOSE:
		if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
		{
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_QUERYENDSESSION:
		if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
		{
			return 1;
		}
		return 0;

	case WM_DESTROY:
		PopFontDeinitialize();
		PostQuitMessage(0);
		return 0;

	default:
		if (messageFindReplace == message)
		{
			pfr = (LPFINDREPLACE)lParam;

			if (pfr->Flags & FR_DIALOGTERM)
			{
				hDlgModeLess = NULL;
			}
			if (pfr->Flags & FR_FINDNEXT)
			{
				if (!PopFindFindText(hwndEdit, &iOffset, pfr))
				{
					OkMessage(hwnd, _T("Text not found!"), _T("\0"));
				}
			}
			if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL)
			{
				if (!PopFindReplaceText(hwndEdit, &iOffset, pfr))
				{
					OkMessage(hwnd, _T("Text not found!"), _T("\0"));
				}
			}
			if (pfr->Flags & FR_REPLACEALL)
			{
				while (PopFindReplaceText(hwndEdit, &iOffset, pfr));
			}
			return 0;
		}
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


static OPENFILENAME ofn;

void PopFileInitialize(HWND hwnd)
{
	static TCHAR szFilter[] = _T("Text Files (*.TXT)\0*.txt\0")
		_T("ASCII Files (*.ASC)\0*.asc\0")
		_T("All Files (*.*)\0*.*\0\0");

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = _T("txt");
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL PopFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return GetOpenFileName(&ofn);
}

BOOL PopFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}

BOOL PopFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
	BYTE		bySwap;
	DWORD		dwBytesRead;
	HANDLE		hFile;
	int			i, iFileLength, iUniTest;
	PBYTE		pBuffer, pText, pConv;

	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
	{
		return FALSE;
	}

	iFileLength = GetFileSize(hFile, NULL);
	pBuffer = malloc(iFileLength + 2);

	ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
	CloseHandle(hFile);
	pBuffer[iFileLength] = '\0';
	pBuffer[iFileLength + 1] = '\0';

	iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
	if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
	{
		pText = pBuffer + 2;
		iFileLength -= 2;

		if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
		{
			for (i = 0; i < iFileLength / 2; i++)
			{
				bySwap = ((BYTE*)pText)[2 * i];
				((BYTE*)pText)[2 * i] = ((BYTE*)pText)[2 * i + 1];
				((BYTE*)pText)[2 * i + 1] = bySwap;
			}
		}

		pConv = malloc(iFileLength + 2);
		_tcscpy((PTSTR)pConv, (PTSTR)pText);
	}
	else
	{
		pText = pBuffer;
		pConv = malloc(2 * iFileLength + 2);
		MultiByteToWideChar(CP_ACP, 0, pText, -1, (PTSTR)pConv, iFileLength + 1);
	}

	SetWindowText(hwndEdit, (PTSTR)pConv);
	free(pBuffer);
	free(pConv);

	return TRUE;
}

BOOL PopFileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
	DWORD		dwBytesWritten;
	HANDLE		hFile;
	int			iLength;
	PTSTR		pstrBuffer;
	WORD		wByteOrderMark = 0xFEFF;

	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
	{
		return FALSE;
	}

	iLength = GetWindowTextLength(hwndEdit);
	pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));
	if (!pstrBuffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);

	GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
	WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR), &dwBytesWritten, NULL);

	if (iLength * sizeof(TCHAR) != dwBytesWritten)
	{
		CloseHandle(hFile);
		free(pstrBuffer);
		return FALSE;
	}

	CloseHandle(hFile);
	free(pstrBuffer);

	return TRUE;
}

#define MAX_STRING_LEN		256
static	TCHAR szFindText[MAX_STRING_LEN];
static	TCHAR szReplText[MAX_STRING_LEN];

HWND PopFindFindDlg(HWND hwnd)
{
	static		FINDREPLACE	fr;
	fr.lStructSize = sizeof(FINDREPLACE);
	fr.hwndOwner = hwnd;
	fr.hInstance = NULL;
	fr.Flags	 = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.lpstrFindWhat = szFindText;
	fr.lpstrReplaceWith = NULL;
	fr.wFindWhatLen = MAX_STRING_LEN;
	fr.wReplaceWithLen = 0;
	fr.lCustData = 0;
	fr.lpfnHook = NULL;
	fr.lpTemplateName = NULL;

	return FindText(&fr);
}

HWND PopFindReplaceDlg(HWND hwnd)
{
	static		FINDREPLACE	fr;
	fr.lStructSize = sizeof(FINDREPLACE);
	fr.hwndOwner = hwnd;
	fr.hInstance = NULL;
	fr.Flags	 = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.lpstrFindWhat = szFindText;
	fr.lpstrReplaceWith = szReplText;
	fr.wFindWhatLen = MAX_STRING_LEN;
	fr.wReplaceWithLen = MAX_STRING_LEN;
	fr.lCustData = 0;
	fr.lpfnHook = NULL;
	fr.lpTemplateName = NULL;

	return ReplaceText(&fr);
}

BOOL PopFindFindText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr)
{
	int		iLength, iPos;
	PTSTR	pstrDoc, pstrPos;

	iLength = GetWindowTextLength(hwndEdit);
	if (NULL == (pstrDoc = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR))))
	{
		return FALSE;
	}
	
	GetWindowText(hwndEdit, pstrDoc, iLength + 1);
	pstrPos = _tcsstr(pstrDoc + *piSearchOffset, pfr->lpstrFindWhat);
	free(pstrDoc);

	if (NULL == pstrPos)
	{
		return FALSE;
	}

	iPos = pstrPos - pstrDoc;
	*piSearchOffset = iPos + _tcslen(pfr->lpstrFindWhat);

	SendMessage(hwndEdit, EM_SETSEL, iPos, *piSearchOffset);
	SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);

	return TRUE;
}

BOOL PopFindNextText(HWND hwndEdit, int* piSearchOffset)
{
	FINDREPLACE fr;
	fr.lpstrFindWhat = szFindText;
	return PopFindFindText(hwndEdit, piSearchOffset, &fr);
}

BOOL PopFindReplaceText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr)
{
	if (!PopFindFindText(hwndEdit, piSearchOffset, pfr))
	{
		return FALSE;
	}

	SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)pfr->lpstrReplaceWith);

	return TRUE;
}

BOOL PopFindValidFind(void)
{
	return *szFindText != '\0';
}

static LOGFONT	logfont;
static HFONT	hFont;

BOOL PopFontChooseFont(HWND hwnd)
{
	CHOOSEFONT cf;

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.hDC = NULL;
	cf.lpLogFont = &logfont;
	cf.iPointSize = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.rgbColors = 0;
	cf.lCustData = 0;
	cf.lpfnHook = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance = NULL;
	cf.lpszStyle = NULL;
	cf.nFontType = 0;
	cf.nSizeMax = 0;
	cf.nSizeMax = 0;
	return ChooseFont(&cf);
}

void PopFontInitialize(HWND hwndEdit)
{
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), (PTSTR)&logfont);
	hFont = CreateFontIndirect(&logfont);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0);
}

void PopFontSetFont(HWND hwndEdit)
{
	HFONT hFontNew;
	RECT rect;

	hFontNew = CreateFontIndirect(&logfont);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontNew, 0);
	DeleteObject(hFont);
	hFont = hFontNew;
	GetClientRect(hwndEdit, &rect);
	InvalidateRect(hwndEdit, &rect, TRUE);
}

void PopFontDeinitialize(void)
{
	DeleteObject(hFont);
}

BOOL bUserAbort;
HWND hDlgPrint;

BOOL CALLBACK PrintDlgProc(HWND hDlg, UINT message, WPARAM wPram, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText(hDlg, szAppName);
		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
		return TRUE;
	case WM_COMMAND:
		bUserAbort = TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		hDlgPrint = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL CALLBACK AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;
	while (!bUserAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (NULL == hDlgPrint || !IsDialogMessage(hDlgPrint, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return !bUserAbort;
}

BOOL PopPrntPrintFile(HINSTANCE hInst, HWND hwnd, HWND hwndEdit, PTSTR pstrTitleName)
{
	static DOCINFO di = {sizeof(DOCINFO)};
	static PRINTDLG pd;
	BOOL bSuccess;
	int yChar, iCharsPerLine, iLinesPerPage, iTotalLines, iTotalPages, iPage, iLine, iLineNum;
	PTSTR pstrBuffer;
	TCHAR szJobName[64 + MAX_PATH];
	TEXTMETRIC tm;
	WORD iColCopy, iNoiColCopy;

	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner = hwnd;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC | PD_NOSELECTION;
	pd.nFromPage = 0;
	pd.nToPage = 0;
	pd.nMinPage = 0;
	pd.nMaxPage = 0;
	pd.nCopies = 1;
	pd.hInstance = 0;
	pd.lpfnPrintHook = NULL;
	pd.lpfnSetupHook = NULL;
	pd.lpPrintTemplateName = NULL;
	pd.lpSetupTemplateName = NULL;
	pd.hPrintTemplate = NULL;
	pd.hSetupTemplate = NULL;

	if (!PrintDlg(&pd))
	{
		return TRUE;
	}
	if (0 == (iTotalLines = SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0)))
	{
		return TRUE;
	}

	GetTextMetrics(pd.hDC, &tm);
	yChar = tm.tmHeight + tm.tmExternalLeading;

	iCharsPerLine = GetDeviceCaps(pd.hDC, HORZRES) / tm.tmAveCharWidth;
	iLinesPerPage = GetDeviceCaps(pd.hDC, VERTRES) / yChar;
	iTotalPages = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage;

	pstrBuffer = malloc(sizeof(TCHAR) * (iCharsPerLine + 1));

	EnableWindow(hwnd, FALSE);
	bSuccess = TRUE;
	bUserAbort = FALSE;

	hDlgPrint = CreateDialog(hInst, _T("PrintDlgBox"), hwnd, PrintDlgProc);
	SetDlgItemText(hDlgPrint, IDC_FILENAME, pstrTitleName);
	SetAbortProc(pd.hDC, AbortProc);

	GetWindowText(hwnd, szJobName, sizeof(szJobName));
	di.lpszDocName = szJobName;

	if (StartDoc(pd.hDC, &di) > 0)
	{
		for (iColCopy = 0; iColCopy < ((WORD)pd.Flags & PD_COLLATE ? pd.nCopies : 1); iColCopy++)
		{
			for (iPage = 0; iPage < iTotalPages; iPage++)
			{
				for (iNoiColCopy = 0; iNoiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies); iNoiColCopy++)
				{
					if (StartPage(pd.hDC) < 0)
					{
						bSuccess = FALSE;
						break;
					}

					for (iLine = 0; iLine < iLinesPerPage; iLine++)
					{
						iLineNum = iLinesPerPage  * iPage + iLine;

						if (iLineNum > iTotalLines)
						{
							break;
						}

						*(int*)pstrBuffer = iCharsPerLine;

						TextOut(pd.hDC, 0, yChar * iLine, pstrBuffer, 
							(int)SendMessage(hwndEdit, EM_GETLINE, (WPARAM)iLineNum, (LPARAM)pstrBuffer));
					}

					if (EndPage(pd.hDC) < 0)
					{
						bSuccess = FALSE;
						break;
					}
					if (bUserAbort)
					{
						break;
					}
				}
				if (!bSuccess || bUserAbort)
				{
					break;
				}
			}
			if (!bSuccess || bUserAbort)
			{
				break;
			}
		}
	}
	else
	{
		bSuccess = FALSE;
	}

	if (bSuccess)
	{
		EndDoc(pd.hDC);
	}

	if (!bUserAbort)
	{
		EnableWindow(hwnd, TRUE);
		DestroyWindow(hDlgPrint);
	}

	free(pstrBuffer);
	DeleteDC(pd.hDC);

	return bSuccess && !bUserAbort;
}

