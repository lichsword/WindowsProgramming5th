#include<windows.h>
#include<tchar.h>
#include "resource.h"

#define INIT_MENU_POS	0
#define HELLO_MENU_POS	2
#define RECT_MENU_POS	1

#define IDM_FIRSTCHILD	50000

LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CloseEnumProc(HWND, LPARAM);
LRESULT CALLBACK HelloWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RectWndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct taghelloData
{
	UINT iColor;
	COLORREF clrText;
}HelloData, *PHelloData;

typedef struct tagRectData
{
	short cxClient;
	short cyClient;
}RectData, *PRectData;

TCHAR szAppName[] = _T("MDIDemo");
TCHAR szFrameClass[] = _T("MDIFrame");
TCHAR szHelloClass[] = _T("MDIHelloChild");
TCHAR szRectClass[] = _T("MDIRectChild");
HINSTANCE hInst;
HMENU hMenuInit, hMenuHello, hMenuRect;
HMENU hMenuInitWindow, hMenuHelloWindow, hMenuRectWindow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HACCEL hAccel;
	HWND hwndFrame, hwndClient;
	MSG msg;
	WNDCLASS wndClass;

	hInst = hInstance;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = FrameWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szFrameClass;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = HelloWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = sizeof(HANDLE);
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szHelloClass;

	RegisterClass(&wndClass);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = RectWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = sizeof(HANDLE);
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szRectClass;

	RegisterClass(&wndClass);

	hMenuInit = LoadMenu(hInstance, _T("MdiMenuInit"));
	hMenuHello = LoadMenu(hInstance, _T("MdiMenuHello"));
	hMenuRect = LoadMenu(hInstance, _T("MdiMenuRect"));

	hMenuInitWindow = GetSubMenu(hMenuInit, INIT_MENU_POS);
	hMenuHelloWindow = GetSubMenu(hMenuHello, HELLO_MENU_POS);
	hMenuRectWindow = GetSubMenu(hMenuRect, RECT_MENU_POS);

	hAccel = LoadAccelerators(hInstance, szAppName);

	hwndFrame = CreateWindow(szFrameClass, 
		_T("MDI Demonstration"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, hMenuInit, hInstance, NULL);
	hwndClient = GetWindow(hwndFrame, GW_CHILD);

	ShowWindow(hwndFrame, iCmdShow);
	UpdateWindow(hwndFrame);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateMDISysAccel(hwndClient, &msg) && !TranslateAccelerator(hwndFrame, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DestroyMenu(hMenuHello);
	DestroyMenu(hMenuRect);

	return msg.wParam;
}

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndClient;
	CLIENTCREATESTRUCT clientcreate;
	HWND hwndChild;
	MDICREATESTRUCT mdicreate;

	switch (message)
	{
	case WM_CREATE:
		clientcreate.hWindowMenu = hMenuInitWindow;
		clientcreate.idFirstChild = IDM_FIRSTCHILD;
		hwndClient = CreateWindow(_T("MDICLIENT"), NULL, 
			WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
			0, 0, 0, 0, hwnd, (HMENU)1, hInst, (PSTR)&clientcreate);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEWHELLO:
			mdicreate.szClass = szHelloClass;
			mdicreate.szTitle = _T("Hello");
			mdicreate.hOwner = hInst;
			mdicreate.x = CW_USEDEFAULT;
			mdicreate.y = CW_USEDEFAULT;
			mdicreate.cx = CW_USEDEFAULT;
			mdicreate.cy = CW_USEDEFAULT;
			mdicreate.style = 0;
			mdicreate.lParam = 0;

			hwndChild = (HWND)SendMessage(hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;

		case IDM_FILE_NEWRECT:
			mdicreate.szClass = szRectClass;
			mdicreate.szTitle = _T("Rectangles");
			mdicreate.hOwner = hInst;
			mdicreate.x = CW_USEDEFAULT;
			mdicreate.y = CW_USEDEFAULT;
			mdicreate.cx = CW_USEDEFAULT;
			mdicreate.cy = CW_USEDEFAULT;
			mdicreate.style = 0;
			mdicreate.lParam = 0;

			hwndChild = (HWND)SendMessage(hwndClient, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;

		case IDM_FILE_CLOSE:
			hwndChild = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
			if (SendMessage(hwndChild, WM_QUERYENDSESSION, 0, 0))
			{
				SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM)hwndChild, 0);
			}
			return 0;

		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		case IDM_WINDOW_TITLE:
			SendMessage(hwndClient, WM_MDITILE, 0, 0);
			return 0;

		case IDM_WINDOW_CASCADE:
			SendMessage(hwndClient, WM_MDICASCADE, 0, 0);
			return 0;

		case IDM_WINDOW_ARRANGE:
			SendMessage(hwndClient, WM_MDIICONARRANGE, 0, 0);
			return 0;

		case IDM_WINDOW_CLOSEALL:
			EnumChildWindows(hwndClient, CloseEnumProc, 0);
			return 0;

		default:
			hwndChild = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
			if (IsWindow(hwndChild))
			{
				SendMessage(hwndChild, WM_COMMAND, wParam, lParam);
			}
			break;
		}
		break;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		SendMessage(hwnd, WM_COMMAND, IDM_WINDOW_CLOSEALL, 0);
		if (NULL != GetWindow(hwndClient, GW_CHILD))
		{
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefFrameProc(hwnd, hwndClient, message, wParam, lParam);
}

BOOL CALLBACK CloseEnumProc(HWND hwnd, LPARAM lParam)
{
	if (GetWindow(hwnd, GW_OWNER))
	{
		return TRUE;
	}

	SendMessage(GetParent(hwnd), WM_MDIRESTORE, (WPARAM)hwnd, 0);
	if (!SendMessage(hwnd, WM_QUERYENDSESSION, 0, 0))
	{
		return TRUE;
	}

	SendMessage(GetParent(hwnd), WM_MDIDESTROY, (WPARAM)hwnd, 0);
	return TRUE;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COLORREF clrTextArray[] = {RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 255)};
	static HWND hwndClient, hwndFrame;
	HDC hdc;
	HMENU hMenu;
	PHelloData pHelloData;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		pHelloData = (PHelloData)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HelloData));
		pHelloData->iColor = IDM_COLOR_BLACK;
		pHelloData->clrText = RGB(0, 0, 0);
		SetWindowLong(hwnd, 0, (long)pHelloData);

		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_COLOR_BLACK:
		case IDM_COLOR_RED:
		case IDM_COLOR_GREEN:
		case IDM_COLOR_BLUE:
		case IDM_COLOR_WHITE:
			pHelloData = (PHelloData)GetWindowLong(hwnd, 0);
			hMenu = GetMenu(hwndFrame);
			CheckMenuItem(hMenu, pHelloData->iColor, MF_UNCHECKED);
			pHelloData->iColor = wParam;
			CheckMenuItem(hMenu, pHelloData->iColor, MF_CHECKED);
			pHelloData->clrText = clrTextArray[wParam - IDM_COLOR_BLACK];
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		pHelloData = (PHelloData)GetWindowLong(hwnd, 0);
		SetTextColor(hdc, pHelloData->clrText);

		GetClientRect(hwnd, &rect);
		DrawText(hdc, _T("Hello, World!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE:
		if (lParam == (LPARAM)hwnd)
		{
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuHello, (LPARAM)hMenuHelloWindow);
		}

		pHelloData = (PHelloData)GetWindowLong(hwnd, 0);
		CheckMenuItem(hMenuHello, pHelloData->iColor, (lParam == (LPARAM)hwnd) ? MF_CHECKED : MF_UNCHECKED);

		if (lParam != (LPARAM)hwnd)
		{
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit, (LPARAM)hMenuInitWindow);
		}

		DrawMenuBar(hwndFrame);
		return 0;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (IDOK != MessageBox(hwnd, _T("OK to close windows?"), _T("Hello"), MB_ICONQUESTION | MB_OKCANCEL))
		{
			return 0;
		}
		break;
	case WM_DESTROY:
		pHelloData = (PHelloData)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pHelloData);
		return 0;
	}

	return DefMDIChildProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK RectWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndClient, hwndFrame;
	HBRUSH hBrush;
	HDC hdc;
	PRectData pRectData;
	PAINTSTRUCT ps;
	int xLeft, xRight, yTop, yBottom;
	short nRed, nGreen, nBlue;

	switch (message)
	{
	case WM_CREATE:
		pRectData = (PRectData)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RectData));
		SetWindowLong(hwnd, 0, (long)pRectData);

		SetTimer(hwnd, 1, 250, NULL);

		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;

	case WM_SIZE:
		if (SIZE_MINIMIZED != wParam)
		{
			pRectData = (PRectData)GetWindowLong(hwnd, 0);
			pRectData->cxClient = LOWORD(lParam);
			pRectData->cyClient = HIWORD(lParam);
		}
		break;

	case WM_TIMER:
		pRectData = (PRectData)GetWindowLong(hwnd, 0);
		xLeft = rand() % pRectData->cxClient;
		xRight = rand() % pRectData->cxClient;
		yTop = rand() % pRectData->cyClient;
		yBottom = rand() % pRectData->cyClient;
		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		hdc = GetDC(hwnd);
		hBrush = CreateSolidBrush(RGB(nRed, nGreen, nBlue));
		SelectObject(hdc, hBrush);

		Rectangle(hdc, min(xLeft, xRight), min(yTop, yBottom), max(xLeft, xRight), max(yTop, yBottom));

		ReleaseDC(hwnd, hdc);
		DeleteObject(hBrush);
		return 0;

	case WM_PAINT:
		InvalidateRect(hwnd, NULL, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE:
		if (lParam == (LPARAM)hwnd)
		{
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuRect, (LPARAM)hMenuRectWindow);
		}
		else
		{
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit, (LPARAM)hMenuInitWindow);
		}

		DrawMenuBar(hwndFrame);
		return 0;

	case WM_DESTROY:
		pRectData = (PRectData)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pRectData);
		KillTimer(hwnd, 1);
		return 0;
	}

	return DefMDIChildProc(hwnd, message, wParam, lParam);
}