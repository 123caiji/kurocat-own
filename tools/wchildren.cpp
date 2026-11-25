#include <windows.h>
#include <cstdio>
static HWND g_settings = nullptr;
static BOOL CALLBACK topCb(HWND h, LPARAM) {
    wchar_t cls[256] = {0}; GetClassNameW(h, cls, 255);
    if (wcsstr(cls, L"TamaSettings")) { g_settings = h; return FALSE; }
    return TRUE;
}
static BOOL CALLBACK childCb(HWND h, LPARAM) {
    wchar_t cls[64] = {0}, txt[128] = {0};
    GetClassNameW(h, cls, 63); GetWindowTextW(h, txt, 127);
    RECT rc{}; GetWindowRect(h, &rc);
    char u8[256] = {0};
    WideCharToMultiByte(CP_UTF8, 0, txt, -1, u8, 255, nullptr, nullptr);
    printf("  %-16ls visible=%d rect=%d,%d %dx%d text=[%s]\n", cls, IsWindowVisible(h) ? 1 : 0,
           (int)rc.left, (int)rc.top, (int)(rc.right - rc.left), (int)(rc.bottom - rc.top), u8);
    return TRUE;
}
int main() {
    EnumWindows(topCb, 0);
    if (!g_settings) { printf("settings window not found\n"); return 1; }
    RECT rc{}; GetWindowRect(g_settings, &rc);
    printf("settings win rect=%d,%d %dx%d\n", (int)rc.left, (int)rc.top, (int)(rc.right - rc.left), (int)(rc.bottom - rc.top));
    EnumChildWindows(g_settings, childCb, 0);
    return 0;
}
