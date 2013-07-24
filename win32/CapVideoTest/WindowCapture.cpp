#include "stdafx.h"

#include "WindowCapture.h"

HBITMAP CaptureDesktop()
{
    HWND hWnd = NULL;

    hWnd = GetDesktopWindow();          // Get handle to desktop window.

    return CaptureWindow(hWnd, FALSE);  // Capture an image of this window.
}

HBITMAP CaptureForegroundWindow(BOOL bClientAreaOnly)
{
    HWND hWnd = NULL;

    hWnd = ::GetForegroundWindow();             // Get the foreground window.

    return CaptureWindow(hWnd, bClientAreaOnly);// Capture an image of this window.
}

HBITMAP CaptureWindow(HWND hWnd, BOOL bClientAreaOnly)
{
    if(!hWnd)
        return NULL;

    HDC hdc;
    RECT rect;
    if(bClientAreaOnly)
    {
        hdc = GetDC(hWnd);
        GetClientRect(hWnd, &rect);
    }
    else
    {
        hdc = GetWindowDC(hWnd);
        GetWindowRect(hWnd, &rect);
    }

    if(!hdc)
        return NULL;

    HDC hMemDC = CreateCompatibleDC(hdc);
    if(hMemDC == NULL)
        return NULL;

    SIZE size;
    size.cx = rect.right - rect.left;
    if(rect.right < rect.left)
        size.cx = -size.cx;
    size.cy = rect.bottom - rect.top;
    if(rect.bottom < rect.top)
        size.cy = -size.cy;

    HBITMAP hDDBmp = CreateCompatibleBitmap(hdc, size.cx, size.cy);
    if(hDDBmp == NULL)
    {
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hdc);
        return NULL;
    }

    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hDDBmp));
    BitBlt(hMemDC, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hdc);

    HBITMAP hBmp = static_cast<HBITMAP>(CopyImage(hDDBmp,
                                                    IMAGE_BITMAP,
                                                    0,
                                                    0,
                                                    LR_CREATEDIBSECTION));


    DeleteObject(hDDBmp);

    return hBmp;
}


void DebugOutString(const char* file,int lineno,const char* fmt,...)
{
	char* pFmt=NULL;
	va_list ap;

	pFmt = new char[2000];

	_snprintf(pFmt,2000,"%s:%d\t",file,lineno);
	OutputDebugString(pFmt);
	AfxMessageBox(pFmt);
	va_start(ap,fmt);
	_vsnprintf(pFmt,2000,fmt,ap);
	OutputDebugString(pFmt);
	AfxMessageBox(pFmt);
	delete [] pFmt;
	return ;
}