#include<windows.h>
#include<TCHAR.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	MessageBox(NULL, _T("Hello, Windows!"), _T("HelloMsg"), 0);

	return 0;
}