#include <windows.h>
#include <cstdio>
static BOOL CALLBACK cb(HWND h, LPARAM) {
    wchar_t title[256] = {0}, cls[256] = {0};
    GetWindowTextW(h, title, 255);
    GetClassNameW(h, cls, 255);
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    wchar_t exe[MAX_PATH] = {0}; DWORD sz = MAX_PATH;
    if (hp) { QueryFullProcessImageNameW(hp, 0, exe, &sz); CloseHandle(hp); }
    const wchar_t* base = wcsrchr(exe, L'\\'); base = base ? base + 1 : exe;
    if (_wcsicmp(base, L"tama.exe") == 0 && wcsstr(cls, L"Settings")) {
        char u8[1024] = {0};
        WideCharToMultiByte(CP_UTF8, 0, title, -1, u8, 1023, nullptr, nullptr);
        printf("class=%ls len=%d\n  utf8=[%s]\n  codes:", cls, (int)wcslen(title), u8);
        for (size_t i = 0; i < wcslen(title); ++i) printf(" %04X", (unsigned)title[i]);
        printf("\n");
    }
    return TRUE;
}
int main() { EnumWindows(cb, 0); return 0; }
