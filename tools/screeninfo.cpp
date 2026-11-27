#include <windows.h>
#include <cstdio>
int main() {
    printf("primary=%dx%d\n", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    printf("virtual=%d,%d %dx%d\n", GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
           GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
    printf("monitors=%d\n", GetSystemMetrics(SM_CMONITORS));
    HDC dc = GetDC(nullptr);
    printf("dpi=%d\n", GetDeviceCaps(dc, LOGPIXELSY));
    ReleaseDC(nullptr, dc);
    return 0;
}
