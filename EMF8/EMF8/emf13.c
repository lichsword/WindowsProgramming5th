#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<commdlg.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("EMF13");
TCHAR szTitle[] = _T("EMF8: Enhanced Metafile Demo #13");

void DrawRuler(HDC hdc, int cx, int cy)
{
	int iAdj, i, iHeight;
	LOGFONT lf;
	TCHAR ch;

	iAdj = GetVersion() & 0x80000000 ? 0 : 1;

	SelectObject(hdc, CreatePen(PS_SOLID, cx / 72 / 6, 0));
	Rectangle(hdc, iAdj, iAdj, cx + iAdj + 1, cy + iAdj + 1);
	for (i =  1; i < 96; i++)
	{
		if (i % 16 == 0)
		{
			iHeight = cy / 2;
		}
		else if (i % 8 == 0)
		{
			iHeight = cy / 3;
		}
		else if (i % 4 == 0)
		{
			iHeight = cy / 5;
		}
		else if (i % 2 == 0)
		{
			iHeight = cy / 8;
		}
		else
		{
			iHeight = cy / 12;
		}
		MoveToEx(hdc, i * cx / 96, cy, NULL);
		LineTo(hdc, i * cx / 96, cy - iHeight);
	}

	FillMemory(&lf, sizeof(lf), 0);
	lf.lfHeight = cy / 2;
	_tcscpy(lf.lfFaceName, _T("Times New Roman"));

	SelectObject(hdc, CreateFontIndirect(&lf));
	SetTextAlign(hdc, TA_BOTTOM | TA_CENTER);
	SetBkMode(hdc, TRANSPARENT);

	for (i = 1; i <= 5; i ++)
	{
		ch = (TCHAR)(i + _T('0'));
		TextOut(hdc, i * cx / 6, cy / 2, &ch, 1);
	}

	DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
	DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
}

void CreateRoutine(HWND hwnd)
{
}

void PaintRoutine(HWND hwnd, HDC hdc, int cxArea, int cyArea)
{
	ENHMETAHEADER emh;
	HENHMETAFILE hemf;
	POINT pt;
	int cxImage, cyImage;
	RECT rect;

	SetMapMode(hdc, MM_HIMETRIC);
	SetViewportOrgEx(hdc, 0, cyArea, NULL);

	pt.x = cxArea;
	pt.y = 0;

	DPtoLP(hdc, &pt, 1);

	hemf = GetEnhMetaFile(_T("emf11.emf"));
	GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
	cxImage = emh.rclBounds.right - emh.rclBounds.left;
	cyImage = emh.rclBounds.bottom - emh.rclBounds.top;

	rect.left = (pt.x - cxImage) / 2;
	rect.right = (pt.x + cxImage) / 2;
	rect.top = (pt.y + cyImage) / 2;
	rect.bottom = (pt.y - cyImage) / 2;

	PlayEnhMetaFile(hdc, hemf, &rect);

	DeleteEnhMetaFile(hemf);
}

HANDLE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szResource[] = _T("EMF");
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

BOOL PrintRoutine(HWND hwnd)
{
	static DOCINFO di;
	static PRINTDLG printdlg = {sizeof(PRINTDLG)};
	static TCHAR szMessage[32];
	BOOL bSuccess = FALSE;
	HDC hdcPrn;
	int cxPage, cyPage;

	printdlg.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;
	if (!PrintDlg(&printdlg))
	{
		return FALSE;
	}
	if (NULL == (hdcPrn = printdlg.hDC))
	{
		return FALSE;
	}

	cxPage = GetDeviceCaps(hdcPrn, HORZRES);
	cyPage = GetDeviceCaps(hdcPrn, VERTRES);

	_tcscpy(szMessage, szAppName);
	_tcscat(szMessage, _T(": Printing"));

	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = szMessage;

	if (StartDoc(hdcPrn, &di) > 0)
	{
		if (StartPage(hdcPrn) > 0)
		{
			PaintRoutine(hwnd, hdcPrn, cxPage, cyPage);

			if (EndPage(hdcPrn) > 0)
			{
				EndDoc(hdcPrn);
				bSuccess = TRUE;
			}
		}
	}

	DeleteDC(hdcPrn);

	return bSuccess;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	BOOL bSuccess;
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		CreateRoutine(hwnd);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_PRINT:
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);

			bSuccess = PrintRoutine(hwnd);

			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			if (!bSuccess)
			{
				MessageBox(hwnd, _T("Error encountered during printing"), szAppName, MB_ICONASTERISK | MB_OK);
			}
			return 0;

		case IDM_APP_ABOUT:
			MessageBox(hwnd, _T("Enhanced Metafile Demo Program\n(c) charles Petzold, 1998"), szAppName, MB_OK);
			return 0;

		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		}

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