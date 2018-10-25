#define OEMRESOURCE
#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("EMF7");
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
		_T("Enhanced Metafile Demo #7"),
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

BOOL CALLBACK EnhMetaFileProc(HDC hdc, HANDLETABLE* pHandleTable, CONST ENHMETARECORD* pEmfRecord, int iHandles, LPARAM pData)
{
	HBRUSH hBrush;
	HPEN hPen;
	LOGBRUSH lb;

	if (EMR_HEADER != pEmfRecord->iType && EMR_EOF != pEmfRecord->iType)
	{
		PlayEnhMetaFileRecord(hdc, pHandleTable, pEmfRecord, iHandles);
	}
	if (EMR_RECTANGLE == pEmfRecord->iType)
	{
		hBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(0, 255, 0);
		lb.lbHatch = 0;

		hPen = SelectObject(hdc, ExtCreatePen(PS_SOLID | PS_GEOMETRIC, 5, &lb, 0, NULL));
		Ellipse(hdc, 100, 100, 200, 200);

		DeleteObject(SelectObject(hdc, hPen));
		SelectObject(hdc, hBrush);
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	ENHMETAHEADER emh;
	HDC hdc, hdcEMF;
	HENHMETAFILE hemf, hemfOld;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		hemfOld = GetEnhMetaFile(_T("emf3.emf"));
		GetEnhMetaFileHeader(hemfOld, sizeof(ENHMETAHEADER), &emh);

		hdcEMF = CreateEnhMetaFile(NULL, _T("emf7.emf"), NULL, _T("EMF7\0EMF Demo #7\0"));
		EnumEnhMetaFile(hdcEMF, hemfOld, EnhMetaFileProc, NULL, (RECT*)&emh.rclBounds);

		hemf = CloseEnhMetaFile(hdcEMF);

		DeleteEnhMetaFile(hemfOld);
		DeleteEnhMetaFile(hemf);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left = rect.right / 4;
		rect.right = 3 * rect.right / 4;
		rect.top = rect.bottom / 4;
		rect.bottom = 3 * rect.bottom / 4;

		hemf = GetEnhMetaFile(_T("emf7.emf"));
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