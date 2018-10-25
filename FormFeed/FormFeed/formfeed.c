#include<windows.h>
#include<tchar.h>
#include<math.h>

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
	static DOCINFO di = {sizeof(DOCINFO), _T("FormFeed")};
	HDC	hdcPrint = GetPrinterDC();

	if (NULL != hdcPrint)
	{
		if (StartDoc(hdcPrint, &di) > 0)
		{
			if (StartPage(hdcPrint) > 0 && EndPage(hdcPrint) > 0)
			{
				EndDoc(hdcPrint);
			}
		}

		DeleteDC(hdcPrint);
	}

	return 0;
}
