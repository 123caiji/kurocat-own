#include <windows.h>
#include <cstdio>
static BOOL CALLBACK cb(HWND h, LPARAM) {
    wchar_t title[256] = {0}, cls[256] = {0};
    GetWindowTextW(h, title, 255);
    GetClassNameW(h, cls, 255);
    if (!title[0] && !cls[0]) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid);
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    wchar_t exe[MAX_PATH] = {0}; DWORD sz = MAX_PATH;
    if (hp) { QueryFullProcessImageNameW(hp, 0, exe, &sz); CloseHandle(hp); }
    const wchar_t* base = wcsrchr(exe, L'\\'); base = base ? base + 1 : exe;
    if (_wcsicmp(base, L"tama.exe") == 0)
        printf("%p  [%ls]  \"%ls\"  visible=%d\n", (void*)h, cls, title, IsWindowVisible(h) ? 1 : 0);
    return TRUE;
}
int main() { EnumWindows(cb, 0); return 0; }
