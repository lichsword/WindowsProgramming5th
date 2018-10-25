#pragma once

#include<windows.h>
#include<tchar.h>

#define NUMLINES ((int)(sizeof sysMetrics / sizeof sysMetrics[0]))

struct
{
	int iIndex;
	TCHAR* szLabel;
	TCHAR* szDesc;
}sysMetrics[] = 
{
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("SM_CXSCREEN"), _T("Screen width in pixels"),
	SM_CYSCREEN, _T("SM_CYSCREEN"), _T("Screen height in pixedls"),
	SM_CXVSCROLL, _T("SM_CXVSCROLL"), _T("Vertical scroll width"),
	SM_CYHSCROLL, _T("SM_CYHSCROLL"), _T("Horizontal scroll height"),
	SM_CYCAPTION, _T("SM_CYCAPTION"), _T("Caption bar height"),
	SM_CXSCREEN, _T("-------------"), _T("---------------------"),
};
