#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<process.h>

typedef struct 
{
	HWND hwnd;
	int cxClient;
	int cyClient;
	int cyChar;
	BOOL bKill;
}PARAMS, *PPARAMS;

LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("Mult2");
	MSG msg;
	WNDCLASS wndClass;
	HWND hwnd;

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

	hwnd = CreateWindow(szAppName,
		_T("Multitasking Demo"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

int CheckBottom(HWND hwnd, int cyClient, int cyChar, int iLine)
{
	if (iLine * cyChar + cyChar > cyClient)
	{
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);

		iLine = 0;
	}
	return iLine;
}

VOID Thread1(PVOID pvoid)
{
	HDC hdc;
	int iNum = 0, iLine = 0;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill)
	{
		if (iNum < 0)
		{
			iNum = 0;
		}

		iLine = CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc = GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, _stprintf(szBuffer, _T("%d"), iNum++));
		ReleaseDC(pparams->hwnd, hdc);
		iLine++;
	}

	_endthread();
}

LRESULT CALLBACK WndProc1(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch (message)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread1, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

VOID Thread2(PVOID pvoid)
{
	HDC hdc;
	int iNum = 1, iLine = 0, i, iSqrt;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill)
	{
		do 
		{
			if (++iNum < 0)
			{
				iNum = 0;
			}

			iSqrt = (int)sqrt(iNum);
			for (i = 2; i <= iSqrt; i++)
			{
				if (iNum % i == 0)
				{
					break;
				}
			}
		} while (i <= iSqrt);

		iLine = CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc = GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, _stprintf(szBuffer, _T("%d"), iNum));
		ReleaseDC(pparams->hwnd, hdc);
		iLine++;
	}

	_endthread();
}

LRESULT CALLBACK WndProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch (message)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread2, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

VOID Thread3(PVOID pvoid)
{
	HDC hdc;
	int iNum = 0, iLine = 0, iNext = 1, iTemp;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill)
	{
		if (iNum < 0)
		{
			iNum = 0;
			iNext = 1;
		}

		iLine = CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc = GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, _stprintf(szBuffer, _T("%d"), iNum));
		ReleaseDC(pparams->hwnd, hdc);

		iTemp = iNum;
		iNum = iNext;
		iNext += iTemp;
		iLine++;
	}

	_endthread();
}

LRESULT CALLBACK WndProc3(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch (message)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread3, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

VOID Thread4(PVOID pvoid)
{
	HDC hdc;
	int iDiameter;
	PPARAMS pparams;

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill)
	{
		InvalidateRect(pparams->hwnd, NULL, TRUE);
		UpdateWindow(pparams->hwnd);

		iDiameter = rand() % (max(1, min(pparams->cxClient, pparams->cyClient)));

		hdc = GetDC(pparams->hwnd);
		Ellipse(hdc, (pparams->cxClient - iDiameter) / 2,
			(pparams->cyClient - iDiameter) / 2,
			(pparams->cxClient + iDiameter) / 2,
			(pparams->cyClient + iDiameter) / 2);
		ReleaseDC(pparams->hwnd, hdc);
	}

	_endthread();
}

LRESULT CALLBACK WndProc4(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PARAMS params;

	switch (message)
	{
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread4, 0, &params);
		return 0;

	case WM_SIZE:
		params.cxClient = LOWORD(lParam);
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndChild[4];
	static TCHAR* szChildClass[] = {_T("Child1"), _T("Child2"), _T("Child3"), _T("Child4")};
	static WNDPROC ChildProc[] = {WndProc1, WndProc2, WndProc3, WndProc4};
	HINSTANCE hInstance;
	int i, cxClient, cyClient;
	WNDCLASS wndclass;

	switch (message)
	{
	case WM_CREATE:
		hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;

		for (i = 0; i < 4; i++)
		{
			wndclass.lpfnWndProc = ChildProc[i];
			wndclass.lpszClassName = szChildClass[i];

			RegisterClass(&wndclass);

			hwndChild[i] = CreateWindow(szChildClass[i], NULL, WS_CHILDWINDOW | WS_BORDER | WS_VISIBLE,
				0, 0, 0, 0, hwnd, (HMENU)i, hInstance, NULL);
		}
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		for (i = 0; i < 4; i++)
		{
			MoveWindow(hwndChild[i], (i % 2) * cxClient / 2, (i > 1) * cyClient / 2,
				cxClient / 2, cyClient / 2, TRUE);
		}
		return 0;

	case WM_CHAR:
		if (VK_ESCAPE == wParam)
		{
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}