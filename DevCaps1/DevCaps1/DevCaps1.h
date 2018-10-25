#pragma once

#include<windows.h>
#include<tchar.h>

#define NUMLINES ((int)(sizeof devcaps / sizeof devcaps[0]))

struct
{
	int iIndex;
	TCHAR* szLabel;
	TCHAR* szDesc;
}devcaps[] = 
{
	HORZSIZE,		_T("HORZSIZE"),		_T("Width in millimeters:"),
	VERTSIZE,		_T("VERTSIZE"),		_T("Height in millimeters:"),
	HORZRES,		_T("HORZRES"),		_T("Width in pixels"),
	VERTRES,		_T("VERTRES"),		_T("Height in raster lines:"),
	BITSPIXEL,		_T("BITSPIXEL"),	_T("Color bits per pixel:"),
	PLANES,			_T("PLANES"),		_T("Number of color planes:"),
	NUMBRUSHES,		_T("NUMBRUSHES"),	_T("Number of device brushes:"),
	NUMPENS,		_T("NUMPENS"),		_T("Number of device pens:"),
	NUMMARKERS,		_T("NUMMARKERS"),	_T("Number of device markers:"),
	NUMFONTS,		_T("NUMFONTS"),		_T("Number of device fonts:"),
	NUMCOLORS,		_T("NUMCOLORS"),	_T("Number of device colors:"),
	PDEVICESIZE,	_T("PDEVICESIZE"),	_T("Size of device structure:"),
	ASPECTX,		_T("ASPECTX"),		_T("Relative width of pixed:"),
	ASPECTY,		_T("ASPECTY"),		_T("Relative height of pixel:"),
	ASPECTXY,		_T("ASPECTXY"),		_T("Relative diagonal of pixel:"),
	LOGPIXELSX,		_T("LOGPIXELSX"),	_T("Horizontal dots per inch:"),
	LOGPIXELSY,		_T("LOGPIXELSY"),	_T("Vertical dots per inch:"),
	SIZEPALETTE,	_T("SIZEPALETTE"),	_T("Number of paldtte entries:"),
	NUMRESERVED,	_T("NUMRESERVED"),	_T("Reserved paldtte entries:"),
	COLORRES,		_T("COLORRES"),		_T("Actual color resolution:"),

};
