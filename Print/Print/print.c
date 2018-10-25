#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL PrintMyPage(HWND);

HINSTANCE hInst;
extern TCHAR szAppName[] = _T("Print");
extern TCHAR szCaption[] = _T("Print Program");

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

HDC GetPrinterDC(void)
{
	DWORD dwNeeded, dwReturned;
	HDC	hdc;
	PRINTER_INFO_4* pinfo4;

	EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
	pinfo4 = malloc(dwNeeded);
	EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
	hdc = CreateDC(NULL, pinfo4->pPrinterName, NULL, NULL);
	free(pinfo4);

	return hdc;
}

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
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hInst = hInstance;
	hWnd = CreateWindow(szAppName,
		szCaption,
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


void PageGDICalls(HDC hdcPrn, int cxPage, int cyPage)
{
	static TCHAR szTextStr[] = _T("Hello, Printer!");
	Rectangle(hdcPrn, 0, 0, cxPage, cyPage);

	MoveToEx(hdcPrn, 0, 0, NULL);
	LineTo(hdcPrn, cxPage, cyPage);
	MoveToEx(hdcPrn, cxPage, 0, NULL);
	LineTo(hdcPrn, 0, cyPage);

	SaveDC(hdcPrn);

	SetMapMode(hdcPrn, MM_ISOTROPIC);
	SetWindowExtEx(hdcPrn, 1000, 1000, NULL);
	SetViewportExtEx(hdcPrn, cxPage / 2, -cyPage / 2, NULL);
	SetViewportOrgEx(hdcPrn, cxPage / 2, cyPage / 2, NULL);

	Ellipse(hdcPrn, -500, 500, 500, -500);

	SetTextAlign(hdcPrn, TA_BASELINE | TA_CENTER);
	TextOut(hdcPrn, 0, 0, szTextStr, _tcslen(szTextStr));
	RestoreDC(hdcPrn, -1);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static int cxClient, cyClient;
	HDC hdc;
	HMENU hMenu;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		hMenu = GetSystemMenu(hwnd, FALSE);
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, 0, 1, _T("&Print"));
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_SYSCOMMAND:
		if (1 == wParam)
		{
			if (!PrintMyPage(hwnd))
			{
				MessageBox(hwnd, _T("Could not print page!"), szAppName, MB_OK | MB_ICONEXCLAMATION);
			}
			return 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		PageGDICalls(hdc, cxClient, cyClient);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL PrintMyPage(HWND hwnd)
{
	static DOCINFO di = {sizeof(DOCINFO), _T("Printer1: Printing")};
	BOOL bSuccess = TRUE;
	HDC hdcPrn;
	int xPage, yPage;

	if (NULL == (hdcPrn = GetPrinterDC()))
	{
		return FALSE;
	}

	xPage = GetDeviceCaps(hdcPrn, HORZRES);
	yPage = GetDeviceCaps(hdcPrn, VERTRES);

	EnableWindow(hwnd , FALSE);
	bUserAbort = FALSE;
	hDlgPrint = CreateDialog(hInst, _T("PrintDlgBox"), hwnd, PrintDlgProc);
	SetAbortProc(hdcPrn, AbortProc);

	if (StartDoc(hdcPrn, &di) > 0)
	{
		if (StartPage(hdcPrn) > 0)
		{
			PageGDICalls(hdcPrn, xPage, yPage);

			if (EndPage(hdcPrn) > 0)
			{
				EndDoc(hdcPrn);
			}
			else
			{
				bSuccess = FALSE;
			}
		}
	}
	else
	{
		bSuccess = FALSE;
	}

	if (!bUserAbort)
	{
		EnableWindow(hwnd, TRUE);
		DestroyWindow(hDlgPrint);
	}
	
	DeleteDC(hdcPrn);

	return bSuccess && !bUserAbort;
}
