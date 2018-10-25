#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<commdlg.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = _T("EmfView");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
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
	wndClass.lpszMenuName = szAppName;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Enhanced Metafile Viewer"),
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

HPALETTE CreatePaletteFromMetaFile(HENHMETAFILE hemf)
{
	HPALETTE hPalette;
	int iNum;
	LOGPALETTE* plp;

	if (!hemf)
	{
		return NULL;
	}
	if (0 == (iNum = GetEnhMetaFilePaletteEntries(hemf, 0, NULL)))
	{
		return NULL;
	}

	plp = malloc(sizeof(LOGPALETTE) + (iNum - 1) * sizeof(PALETTEENTRY));
	plp->palVersion = 0x300;
	plp->palNumEntries = iNum;

	GetEnhMetaFilePaletteEntries(hemf, iNum, plp->palPalEntry);
	hPalette = CreatePalette(plp);
	free(plp);

	return hPalette;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static DOCINFO di = {sizeof(DOCINFO), _T("EmfView: Printing")};
	static HENHMETAFILE hemf;
	static OPENFILENAME ofn;
	static PRINTDLG printdlg = {sizeof(PRINTDLG)};
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static TCHAR szFilter[] = _T("Enhanced Metafiles (*.EMF)\0*.emf\0All Files (*.*)\0*.*\0\0");
	BOOL bSuccess;
	ENHMETAHEADER header;
	HDC hdc, hdcPrn;
	HENHMETAFILE hemfCopy;
	HMENU hMenu;
	HPALETTE hPalette;
	int i, iLength, iEnable;
	PAINTSTRUCT ps;
	RECT rect;
	PTSTR pBuffer;

	switch (message)
	{
	case WM_CREATE:
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.hInstance = NULL;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrCustomFilter = NULL;
		ofn.nMaxCustFilter = 0;
		ofn.nFilterIndex = 0;
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFileTitle = szTitleName;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = NULL;
		ofn.Flags = 0;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = _T("emf");
		ofn.lCustData = 0;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;

		return 0;

	case WM_INITMENUPOPUP:
		hMenu = GetMenu(hwnd);
		iEnable = hemf ? MF_ENABLED : MF_GRAYED;

		EnableMenuItem(hMenu, IDM_FILE_SAVE_AS,		iEnable);
		EnableMenuItem(hMenu, IDM_FILE_PRINT,		iEnable);
		EnableMenuItem(hMenu, IDM_FILE_PROPERTIES,	iEnable);
		EnableMenuItem(hMenu, IDM_EDIT_CUT,			iEnable);
		EnableMenuItem(hMenu, IDM_EDIT_COPY,		iEnable);
		EnableMenuItem(hMenu, IDM_EDIT_DELETE,		iEnable);

		EnableMenuItem(hMenu, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_ENHMETAFILE) ? MF_ENABLED : MF_GRAYED);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			ofn.Flags = 0;
			if (!GetOpenFileName(&ofn))
			{
				return 0;
			}

			if (NULL != hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf = NULL;
			}

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);

			hemf = GetEnhMetaFile(szFileName);

			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			InvalidateRect(hwnd, NULL, TRUE);

			if (NULL == hemf)
			{
				MessageBox(hwnd, _T("Canno tload metafile"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			}
			return 0;

		case IDM_FILE_SAVE_AS:
			if (NULL == hemf)
			{
				return 0;
			}
			ofn.Flags = OFN_OVERWRITEPROMPT;
			if (!GetSaveFileName(&ofn))
			{
				return 0;
			}

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			hemfCopy = CopyEnhMetaFile(hemf, szFileName);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			if (NULL != hemfCopy)
			{
				DeleteEnhMetaFile(hemf);
			    hemf = hemfCopy;
			}
			else
			{
				MessageBox(hwnd, _T("Cannot save metafile"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			}

			return 0;

		case IDM_FILE_PRINT:
			printdlg.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;
			if (!PrintDlg(&printdlg))
			{
				return 0;
			}
			if (NULL == (hdcPrn = printdlg.hDC))
			{
				MessageBox(hwnd, _T("Cannot obtain printer DC"), szAppName, MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}

			rect.left = 0;
			rect.right = GetDeviceCaps(hdcPrn, HORZRES);
			rect.top = 0;
			rect.bottom = GetDeviceCaps(hdcPrn, VERTRES);

			bSuccess = FALSE;

			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);

			if ((StartDoc(hdcPrn, &di) > 0) && (StartPage(hdcPrn) > 0))
			{
				PlayEnhMetaFile(hdcPrn, hemf, &rect);

				if (EndPage(hdcPrn) > 0)
				{
					bSuccess = TRUE;
					EndDoc(hdcPrn);
				}
			}
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			DeleteDC(hdcPrn);
			if (!bSuccess)
			{
				MessageBox(hwnd, _T("Could not print metafile"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			}
			return 0;

		case IDM_FILE_PROPERTIES:
			if (NULL == hemf)
			{
				return 0;
			}
			iLength = GetEnhMetaFileDescription(hemf, 0, NULL);
			pBuffer = malloc((iLength + 256) * sizeof(TCHAR));

			GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &header);

			i = _stprintf(pBuffer, _T("Bounds = (%i, %i) to (%i, %i) pixels\n"), 
				header.rclBounds.left, header.rclBounds.top, header.rclBounds.right, header.rclBounds.bottom);
			i += _stprintf(pBuffer + i, _T("Frame = (%i, %i) to (%i, %i) mms\n"),
				header.rclFrame.left, header.rclFrame.top, header.rclFrame.right, header.rclFrame.bottom);
			i += _stprintf(pBuffer + i, _T("Resolution = (%i, %i) pixels = (%i, %i)mms\n"),
				header.szlDevice.cx, header.szlDevice.cy, header.szlMillimeters.cx, header.szlMillimeters.cy);
			i += _stprintf(pBuffer + i, _T("Size = %i, Records = %i, Handles = %i, Palette entries = %i\n"),
				header.nBytes, header.nRecords, header.nHandles, header.nPalEntries);

			if (0 != iLength)
			{
				i += _stprintf(pBuffer + i, _T("Description = "));
				GetEnhMetaFileDescription(hemf, iLength, pBuffer + i);
				pBuffer[_tcslen(pBuffer)] = '\t';
			}

			MessageBox(hwnd, pBuffer, _T("Metafile Properties"), MB_OK);
			free(pBuffer);

			return 0;

		case IDM_EDIT_COPY:
		case IDM_EDIT_CUT:
			if (NULL == hemf)
			{
				return 0;
			}

			hemfCopy = CopyEnhMetaFile(hemf, NULL);
			OpenClipboard(hwnd);
			EmptyClipboard();
			SetClipboardData(CF_ENHMETAFILE, hemfCopy);
			CloseClipboard();

			if (LOWORD(wParam) == IDM_EDIT_COPY)
			{
				return 0;
			}
			//fall through if IDM_EDIT_CUT
			
		case IDM_EDIT_DELETE:
			if (NULL != hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf = NULL;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return 0;

		case IDM_EDIT_PASTE:
			OpenClipboard(hwnd);
			hemfCopy = GetClipboardData(CF_ENHMETAFILE);

			CloseClipboard();
			if (NULL != hemfCopy && NULL != hemf)
			{
				DeleteEnhMetaFile(hemf);
				hemf = NULL;
			}
			hemf = CopyEnhMetaFile(hemfCopy, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case IDM_APP_ABOUT:
			MessageBox(hwnd, _T("Enhanced Metafile Viewer\n(c) charles Petzold, 1998"), szAppName, MB_OK);
			return 0;
			
		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		}

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		if (NULL != hemf)
		{
			if (hPalette = CreatePaletteFromMetaFile(hemf))
			{
				SelectPalette(hdc, hPalette, FALSE);
				RealizePalette(hdc);
			}
			GetClientRect(hwnd, &rect);
			PlayEnhMetaFile(hdc, hemf, &rect);

			if (hPalette)
			{
				DeleteObject(hPalette);
			}
		}

		EndPaint(hwnd, &ps);
		return 0;

	case WM_QUERYNEWPALETTE:
		if (NULL == hemf || (NULL == (hPalette = CreatePaletteFromMetaFile(hemf))))
		{
			return FALSE;
		}

		hdc = GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		InvalidateRect(hwnd, NULL, FALSE);

		DeleteObject(hPalette);
		ReleaseDC(hwnd, hdc);
		return TRUE;

	case WM_PALETTECHANGED:
		if ((HWND)wParam == hwnd)
		{
			break;
		}
		if (NULL == hemf || (NULL == (hPalette = CreatePaletteFromMetaFile(hemf))))
		{
			break;
		}
		hdc = GetDC(hwnd);
		SelectPalette(hdc, hPalette, FALSE);
		RealizePalette(hdc);
		UpdateColors(hdc);

		DeleteObject(hPalette);
		ReleaseDC(hwnd, hdc);
		break;

	case WM_DESTROY:
		if (NULL != hemf)
		{
			DeleteEnhMetaFile(hemf);
		}
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}