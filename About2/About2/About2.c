#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int iCurrentColor = IDC_BLACK;
int iCurrentFigure = IDC_RECTANGLE;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("About2");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

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
		_T("About Box Demo Program"),
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

void PaintWindow(HWND hwnd, int iColor, int iFigure)
{
	static COLORREF crColor[8] = {
		RGB(0, 0, 0), RGB(0, 0, 255),
		RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255, 0, 0), RGB(255, 0, 255),
		RGB(255, 255, 0), RGB(255, 255, 255)
	};
	HBRUSH			hBrush;
	HDC				hdc;
	RECT			rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (IDC_RECTANGLE == iFigure)
	{
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	}
	else
	{
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	}

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE	hInstance;
	PAINTSTRUCT			ps;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			if (IDOK == DialogBox(hInstance, _T("AboutBox"), hwnd, AboutDlgProc))
			{
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;
		}
		break;

	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

		PaintWindow(hwnd, iCurrentColor, iCurrentFigure);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND		hCtrlBlock;
	static int		iColor, iFigure;

	switch (message)
	{
	case WM_INITDIALOG:
		iColor = iCurrentColor;
		iFigure = iCurrentFigure;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hDlg, IDC_RECTANGLE, IDC_ELLIPSE, iFigure);

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		SetFocus(GetDlgItem(hDlg, iColor));

		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor = iColor;
			iCurrentFigure = iFigure;
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;

		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
			iColor = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;

		case IDC_RECTANGLE:
		case IDC_ELLIPSE:
			iFigure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECTANGLE, IDC_ELLIPSE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;
		}
		break;

	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, iColor, iFigure);
		break;
	}
	return FALSE;
}