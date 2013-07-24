#ifndef WINDOWCAPTURE_H
#define WINDOWCAPTURE_H


HBITMAP CaptureDesktop();
HBITMAP CaptureForegroundWindow(BOOL bClientAreaOnly = FALSE);
HBITMAP CaptureWindow(HWND hwnd, BOOL bClientAreaOnly = FALSE);

#ifdef __cplusplus
extern "C" {
#endif

#define  DEBUG_INFO(fmt,...) DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__)

void DebugOutString(const char* file,int lineno,const char* fmt,...);

#ifdef __cplusplus
}
#endif


#endif  WINDOWCAPTURE_H