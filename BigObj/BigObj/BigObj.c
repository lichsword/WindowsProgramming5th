#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<process.h>

#define REP		1000000
#define STATUS_READY	0
#define STATUS_WORKING	1
#define STATUS_DONE		2

#define WM_CALC_DONE	(WM_USER + 0)
#define WM_CALC_ABORTED	(WM_USER + 1)

typedef struct
{
	HWND hwnd;
	BOOL bContinue;
}PARAMS, *PPARAMS;

LRESULT APIENTRY WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("BigJob1");
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
		_T("Multithreading Demo"),
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

VOID Thread(PVOID pvoid)
{
	double A = 1.0;
	INT i;
	LONG lTime;
	volatile PPARAMS pparams;

	pparams = (PPARAMS)pvoid;
	lTime = GetCurrentTime();

	for (i = 0; i < REP && pparams->bContinue; i++)
	{
		A = tan(atan(exp(log(sqrt(A * A))))) + 1.0;
	}
	if (i == REP)
	{
		lTime = GetCurrentTime() - lTime;
		SendMessage(pparams->hwnd, WM_CALC_DONE, 0, lTime);
	}
	else
	{
		SendMessage(pparams->hwnd, WM_CALC_ABORTED, 0, 0);
	}

	_endthread();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT iStatus;
	static LONG lTime;
	static PARAMS params;
	static TCHAR* szmessage[] = {
		_T("Ready (left mouse button begins)"),
		_T("Working (right mouse button ends"),
		_T("%d repetitions in %ld msec")
	};
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	TCHAR szBuffer[64];

	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (STATUS_WORKING == iStatus)
		{
			MessageBeep(0);
			return 0;
		}
		iStatus = STATUS_WORKING;
		params.hwnd = hwnd;
		params.bContinue = TRUE;

		_beginthread(Thread, 0, &params);

		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_RBUTTONDOWN:
		params.bContinue = FALSE;
		return 0;

	case WM_CALC_DONE:
		lTime = lParam;
		iStatus = STATUS_DONE;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_CALC_ABORTED:
		iStatus = STATUS_READY;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		_stprintf(szBuffer, szmessage[iStatus], REP, lTime);
		DrawText(hdc, szBuffer, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}