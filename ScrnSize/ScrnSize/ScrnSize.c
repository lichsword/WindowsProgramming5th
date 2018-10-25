#include<windows.h>
#include<TCHAR.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	int cxScreen, cyScreen;
	TCHAR szBuffer[256]= {0};

	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
	_stprintf(szBuffer, _T("The screen is %d pixels wide by %d pixels high."), cxScreen, cyScreen);
	MessageBox(NULL, szBuffer, _T("ScrnSize"), 0);

	return 0;
}