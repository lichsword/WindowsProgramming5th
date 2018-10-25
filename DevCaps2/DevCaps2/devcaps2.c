#include<windows.h>
#include<tchar.h>
#include<math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DoBasicInfo(HDC, HDC, int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);

typedef struct  
{
    int iMask;
    TCHAR* szDesc;
}BITS;

#define IDM_DEVMODE        1000

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = _T("DevCaps2");
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
        NULL,
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static TCHAR        szDevice[32], szWindowText[64];
    static int            cxChar, cyChar, nCurrentDevice = IDM_SCREEN, nCurrentInfo = IDM_BASIC;
    static DWORD        dwNeeded, dwReturned;
    static PRINTER_INFO_4*    pInfo4;
    DWORD                i;
    HDC                    hdc, hdcInfo;
    HMENU                hMenu;
    HANDLE                hPrint;
    PAINTSTRUCT            ps;
    TEXTMETRIC            tm;

    switch (message)
    {
    case WM_CREATE:
        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;
        cyChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc);
        //fall through

    case WM_SETTINGCHANGE:
        hMenu = GetSubMenu(GetMenu(hwnd), 0);
        while (GetMenuItemCount(hMenu) > 1)
        {
            DeleteMenu(hMenu, 1, MF_BYPOSITION);
        }

        EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
        pInfo4 = malloc(dwNeeded);
        EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pInfo4, dwNeeded, &dwNeeded, &dwReturned);
        for (i = 0; i < dwReturned; i++)
        {
            AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pInfo4[i].pPrinterName);
        }
        free(pInfo4);

        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, 0, IDM_DEVMODE, _T("Properties"));
        wParam = IDM_SCREEN;
        //fall through

    case WM_COMMAND:
        hMenu = GetMenu(hwnd);
        if (LOWORD(wParam) == IDM_SCREEN || LOWORD(wParam) < IDM_DEVMODE)
        {
            CheckMenuItem(hMenu, nCurrentDevice, MF_UNCHECKED);
            nCurrentDevice = LOWORD(wParam);
            CheckMenuItem(hMenu, nCurrentDevice, MF_CHECKED);
        }
        else if (LOWORD(wParam) == IDM_DEVMODE)
        {
            GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice) / sizeof(TCHAR), MF_BYCOMMAND);

            if (OpenPrinter(szDevice, &hPrint, NULL))
            {
                PrinterProperties(hwnd, hPrint);
                ClosePrinter(hPrint);
            }
        }
        else
        {
            CheckMenuItem(hMenu, nCurrentInfo, MF_UNCHECKED);
            nCurrentInfo = LOWORD(wParam);
            CheckMenuItem(hMenu, nCurrentInfo, MF_CHECKED);
        }
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_INITMENUPOPUP:
        if (0 == lParam)
        {
            EnableMenuItem(GetMenu(hwnd), IDM_DEVMODE, nCurrentDevice == IDM_SCREEN ? MF_GRAYED : MF_ENABLED);
        }
        return 0;

    case WM_PAINT:
        _tcscpy(szWindowText, _T("Device Capabilities"));
        if (IDM_SCREEN == nCurrentDevice)
        {
            _tcscpy(szDevice, _T("DISPLAY"));
            hdcInfo = CreateIC(szDevice, NULL, NULL, NULL);
        }
        else
        {
            hMenu = GetMenu(hwnd);
            GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice), MF_BYCOMMAND);
            hdcInfo = CreateIC(NULL, szDevice, NULL, NULL);
        }
        _tcscat(szWindowText, szDevice);
        SetWindowText(hwnd, szWindowText);

        hdc  = BeginPaint(hwnd, &ps);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

        if (hdcInfo)
        {
            switch (nCurrentInfo)
            {
            case IDM_BASIC:
                DoBasicInfo(hdc, hdcInfo, cxChar, cyChar);
                break;
                
            case IDM_OTHER:
                DoOtherInfo(hdc, hdcInfo, cxChar, cyChar);
                break;

            case IDM_CURVE:
            case IDM_LINE:
            case IDM_POLY:
            case IDM_TEXT:
                DoBitCodedCaps(hdc, hdcInfo, cxChar, cyChar, nCurrentInfo - IDM_CURVE);
                break;
            }
            DeleteDC(hdcInfo);
        }
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void DoBasicInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
    static struct 
    {
        int nIndex;
        TCHAR* szDesc;
    }info[] = 
    {
        HORZSIZE,        _T("HORZSIZE        Width in millimeters:"),
        VERTSIZE,        _T("VERTSIZE        Height in millimeters:"),
        HORZRES,         _T("HORZRS          Width in pixels:"),
        VERTRES,         _T("VERTRES         Height in raster lines:"),
        BITSPIXEL,       _T("BITSPIXEL       Color bits per pixel:"),
        PLANES,          _T("PLANES          Number of color planes:"),
        NUMBRUSHES,      _T("NUMBRUSHES      Number of device brushes:"),
        NUMPENS,         _T("NUMPENS         Number of device pens:"),
        NUMMARKERS,      _T("NUMMARKERS      Number of device markers:"),
        NUMFONTS,        _T("NUMFONTS        Number of device fonts:"),
        NUMCOLORS,       _T("NUMCOLORS       Number of device colors:"),
        PDEVICESIZE,     _T("PDEVICESIZE     Size of device structure:"),
        ASPECTX,         _T("ASPECTX         Relative width of pixel:"),
        ASPECTY,         _T("ASPECTY         Relative height of pixel:"),
        ASPECTXY,        _T("ASPECTXY        Relative diagonal of pixel:"),
        LOGPIXELSX,      _T("LOGPIXELSX      Horizontal dots per inch:"),
        LOGPIXELSY,      _T("LOGPIXELSY      Vertical dots per inch:"),
        SIZEPALETTE,     _T("SIZEPALETTE     Number of palette entries:"),
        NUMRESERVED,     _T("NUMRESERVED     Reserved palette entries:"),
        COLORRES,        _T("COLORRES        Actual color resolution:"),
        PHYSICALWIDTH,   _T("PHYSICALWIDTH   Printer page pixel width:"),
        PHYSICALHEIGHT,  _T("PHYSICALHEIGHT  Printer page pixel height:"),
        PHYSICALOFFSETX, _T("PHYSICALOFFSETX Printer Page x Offset:"),
        PHYSICALOFFSETY, _T("PHYSICALOFFSETY Printer Page y offset:")
    };
    int i;
    TCHAR szBuffer[80];

    for (i = 0; i < sizeof(info) / sizeof(info[0]); i++)
    {
        TextOut(hdc, cxChar, (i + 1) * cyChar, szBuffer,
            _stprintf(szBuffer, _T("%-45s%8d"), info[i].szDesc, GetDeviceCaps(hdcInfo, info[i].nIndex)));
    }
}

void DoOtherInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
    static BITS clip[] = 
    {
        CP_RECTANGLE,        _T("CP_RECTANGLE Can Clip To Rectangle:")
    };

    static BITS raster[] = 
    {
        RC_BITBLT,         _T("RC_BITBLT             Capable of simple Bitblt:"),
        RC_BANDING,        _T("RC_BANDING            Requires banding support:"),
        RC_SCALING,        _T("RC_SCALING            Requires scaling support:"),
        RC_BITMAP64,       _T("RC_BITMAP64           Supports bitmaps >64K:"),
        RC_GDI20_OUTPUT,   _T("RC_GDI20_OUTPU        Has 2.0 output calls:"),
        RC_DI_BITMAP,      _T("RC_DI_BITMAP          Supports DIB to memory:"),
        RC_PALETTE,        _T("RC_PALETTE            Supports a palette:"),
        RC_DIBTODEV,       _T("RC_DIBTODEV           Supports bitmap conversion:"),
        RC_BIGFONT,        _T("RC_BIGFONT            Supports fonts >64K:"),
        RC_STRETCHBLT,     _T("RC_STRETCHBLT         Supports StretchBlt:"),
        RC_FLOODFILL,      _T("RC_FLOODFILL          Supports FloodFill:"),
        RC_STRETCHDIB,     _T("RC_STRETCHDIB         Supports StretchDIBits:")
    };

    static TCHAR* szTech[] = {
        _T("DT_PLOTTER (Vector plotter)"),
        _T("DT_RASDISPLAY (Raster display)"),
        _T("DT_RASPRINTER (Raster pritner)"),
        _T("DT_RASCAMERA (Raster camera)"),
        _T("DT_CHARSTREAM (Character stream)"),
        _T("DT_METAFILE (Metafile)"),
        _T("DT_DISPFILE (Display file)")
    };
    int i;
    TCHAR    szBuffer[80];

    TextOut(hdc, cxChar, cyChar, szBuffer, 
        _stprintf(szBuffer, _T("%-24s%04XH"), _T("DRIVERVERSION:"), GetDeviceCaps(hdcInfo, DRIVERVERSION)));
    TextOut(hdc, cxChar, 2 * cyChar, szBuffer, 
        _stprintf(szBuffer, _T("%-24s%-40s"), _T("TECHNOLOGY:"), szTech[GetDeviceCaps(hdcInfo, TECHNOLOGY)]));
    TextOut(hdc, cxChar, 4 * cyChar, szBuffer, 
        _stprintf(szBuffer, _T("CLIPCAPS (Clipping capabilities)")));
    for (i = 0; i < sizeof(clip) / sizeof(clip[0]); i++)
    {
        TextOut(hdc, 9 * cxChar, (i + 6) * cyChar, szBuffer, 
            _stprintf(szBuffer, _T("%-45s %3s"), clip[i].szDesc, 
            GetDeviceCaps(hdcInfo, CLIPCAPS) & clip[i].iMask ? _T("Yes") : _T("No")));
    }
    TextOut(hdc, cxChar, 8 * cyChar, szBuffer, 
        _stprintf(szBuffer, _T("RASTERCAPS (Raster capabilities)")));
    for (i = 0; i < sizeof(raster) / sizeof(raster[0]); i++)
    {
        TextOut(hdc, 9 * cxChar, (i + 10) * cyChar, szBuffer, 
            _stprintf(szBuffer, _T("%-45s %3s"), raster[i].szDesc, 
            GetDeviceCaps(hdcInfo, RASTERCAPS) & raster[i].iMask ? _T("Yes") : _T("No")));
    }
}

void DoBitCodedCaps(HDC hdc, HDC hdcInfo, int cxChar, int cyChar, int iType)
{
    static BITS curves[] =
    {
        CC_CIRCLES,            _T("CC_CIRCLES            Can do circles:"),
        CC_PIE,                _T("CC_PIE                Can do pie wedges:"),
        CC_CHORD,              _T("CC_CHORD              Can do chord arcs:"),
        CC_ELLIPSES,           _T("CC_ELLIPSES           Can do ellipses:"),
        CC_WIDE,               _T("CC_WIDE               Can do wide borders:"),
        CC_STYLED,             _T("CC_STYLED             Can do styled borders:"),
        CC_WIDESTYLED,         _T("CC_WIDESTYLED         Can do wide and styled borders:"),
        CC_INTERIORS,          _T("CC_INTERIORS          Can do interiors:"),
    };
    static BITS lines[] = 
    {
        LC_POLYLINE,        _T("LC_POLYLINE            Can do polyline:"),
        LC_MARKER,          _T("LC_MARKER              Can do markers:"),
        LC_POLYMARKER,      _T("LC_POLYMARKER          Can do polymarkers:"),
        LC_WIDE,            _T("LC_WIDE                Can do wide lines:"),
        LC_STYLED,          _T("LC_STYLED              Can do styled lines:"),
        LC_WIDESTYLED,      _T("LC_WIDESTYLED          Can do wide and styled lines:"),
        LC_INTERIORS,      _T("LC_INTERIORS            Can do interiors:"),
    };
    static BITS poly[] =
    {
        PC_POLYGON,            _T("PC_POLYGON            Can do alternate fill polygon:"),
        PC_RECTANGLE,          _T("PC_RECTANGLE          Can do rectangle:"),
        PC_WINDPOLYGON,        _T("PC_WINDPOLYGON        Can do winding number fill polygon:"),
        PC_SCANLINE,           _T("PC_SCANLINE           Can do scanlines:"),
        PC_WIDE,               _T("PC_WIDE               Can do wide borders:"),
        PC_STYLED,             _T("PC_STYLED             Can do styled borders:"),
        PC_WIDESTYLED,         _T("PC_WIDESTYLED         Can do wide and styled borders:"),
        PC_INTERIORS,          _T("PC_INTERIORS          Can do interiors:"),
    };
    static BITS text[] = 
    {
        TC_OP_CHARACTER,      _T("TC_OP_CHARACTER        Can do character output precision:"),
        TC_OP_STROKE,         _T("TC_OP_STROKE           Can do stroke output precision:"),
        TC_CP_STROKE,         _T("TC_CP_STROKE           Can do stroke clip precision:"),
        TC_CR_90,             _T("TC_CR_90               Can do 90 degree character rotation:"),
        TC_CR_ANY,            _T("TC_CR_ANY              Can do any character rotation:"),
        TC_SF_X_YINDEP,       _T("TC_SF_X_YINDEP         Can do scaling independent of X and Y:"),
        TC_SA_DOUBLE,         _T("TC_SA_DOUBLE           Can do doubled character for scaling:"),
        TC_SA_INTEGER,        _T("TC_SA_INTEGER          Can do integer multiples for scaling:"),
        TC_SA_CONTIN,         _T("TC_SA_CONTIN           Can do any multiples for exact scaling:"),
        TC_EA_DOUBLE,         _T("TC_EA_DOUBLE           Can do double weight characters:"),
        TC_IA_ABLE,           _T("TC_IA_ABLE             Can do italicizing:"),
        TC_UA_ABLE,           _T("TC_UA_ABLE             Can do underlining:"),
        TC_SO_ABLE,           _T("TC_SO_ABLE             Can do strikeouts:"),
        TC_RA_ABLE,           _T("TC_RA_ABLE             Can do raster fonts:"),
        TC_VA_ABLE,           _T("TC_VA_ABLE             Can do vector fonts:")
    };

    static struct
    {
        int iIndex;
        TCHAR* szTitle;
        BITS (*pbits)[];
        int iSize;
    }
    bitinfo[] =
    {
        CURVECAPS,        _T("CURVCAPS (Curve Capabilities)"),            (BITS (*)[])curves, sizeof(curves) / sizeof(curves[0]),
        LINECAPS,        _T("LINECAPS (Line Capabilities)"),                (BITS (*)[])lines, sizeof(lines) / sizeof(lines[0]),
        POLYGONALCAPS,    _T("POLYGONALCAPS (Polygonal Capabilities)"),    (BITS (*)[])poly, sizeof(poly) / sizeof(poly[0]),
        TEXTCAPS,        _T("TEXTCAPS (TEXT Capabilities)"),                (BITS (*)[])text, sizeof(text) / sizeof(text[0])
    };

    static TCHAR szBuffer[80];
    BITS (*pbits)[] = bitinfo[iType].pbits;
    int i, iDevCaps = GetDeviceCaps(hdcInfo, bitinfo[iType].iIndex);

    TextOut(hdc, cxChar, cyChar, bitinfo[iType].szTitle, _tcslen(bitinfo[iType].szTitle));
    for (i = 0; i < bitinfo[iType].iSize; i++)
    {
        TextOut(hdc, cxChar, (i + 3) * cyChar, szBuffer,
            _stprintf(szBuffer, _T("%-55s %3s"), (*pbits)[i].szDesc,
            iDevCaps & (*pbits)[i].iMask ? _T("Yes") : _T("No")));
    }
}