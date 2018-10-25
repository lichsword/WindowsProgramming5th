#include<windows.h>
#include<tchar.h>

#define IDM_SYS_ABOUT		1
#define IDM_SYS_HELP		2
#define IDM_SYS_REMOVE		3

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("PoorMenu");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	HMENU hMenu;

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
		_T("The Poor-Person's Menu"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	hMenu = GetSystemMenu(hWnd, FALSE);
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, IDM_SYS_ABOUT, _T("About..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_HELP, _T("Help..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_REMOVE, _T("Remove additions"));

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
	switch (message)
	{
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_SYS_ABOUT:
			MessageBox(hwnd, _T("A Poor-person's Menu Program\n(c) Charles Petzold, 1998"), szAppName, MB_OK | MB_ICONINFORMATION);
			return 0;

		case IDM_SYS_HELP:
			MessageBox(hwnd, _T("Hlep not yet implemented!"), szAppName, MB_OK | MB_ICONEXCLAMATION);
			return 0;

		case IDM_SYS_REMOVE:
			GetSystemMenu(hwnd, TRUE);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
