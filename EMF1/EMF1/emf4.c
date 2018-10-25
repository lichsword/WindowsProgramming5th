#define OEMRESOURCE
#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("EMF4");
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

	hWnd = CreateWindow(szAppName,
		_T("Enhanced Metafile Demo #4"),
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	BITMAP bm;
	HBITMAP hbm;
	static HENHMETAFILE hemf;
	HDC hdc, hdcEMF, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	DWORD winVer;

	switch (message)
	{
	case WM_CREATE:
		hdcEMF = CreateEnhMetaFile(NULL, _T("emf4.emf"), NULL, _T("EMF4\0EMF Demo #4\0"));

		hbm = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CLOSE));
		GetObject(hbm, sizeof(BITMAP), &bm);
		hdcMem = CreateCompatibleDC(hdcEMF);
		SelectObject(hdcMem, hbm);
		StretchBlt(hdcEMF, 100, 100, 100, 100,
			hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

		DeleteDC(hdcMem);
		DeleteObject(hbm);

		hemf = CloseEnhMetaFile(hdcEMF);
		DeleteEnhMetaFile(hemf);

		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left = rect.right / 4;
		rect.right = 3 * rect.right / 4;
		rect.top = rect.bottom / 4;
		rect.bottom = 3 * rect.bottom / 4;

		hemf = GetEnhMetaFile(_T("emf4.emf"));
		PlayEnhMetaFile(hdc, hemf, &rect);
		DeleteEnhMetaFile(hemf);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}