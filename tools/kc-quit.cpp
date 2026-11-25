#include <windows.h>
#include <cstdio>
#include <cstring>
static const UINT WM_REQUEST_QUIT = WM_APP + 5;
static int sent = 0;
static BOOL CALLBACK cb(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hp) return TRUE;
    wchar_t exe[MAX_PATH] = {0};
    DWORD sz = MAX_PATH;
    QueryFullProcessImageNameW(hp, 0, exe, &sz);
    CloseHandle(hp);
    const wchar_t* name = wcsrchr(exe, L'\\');
    name = name ? name + 1 : exe;
    if (_wcsnicmp(name, L"kurocat-own", 11) == 0 || _wcsnicmp(name, L"kc-demo", 7) == 0) {
        PostMessageW(h, WM_REQUEST_QUIT, 0, 0);
        PostMessageW(h, WM_CLOSE, 0, 0);
        printf("posted %ls\n", name);
        sent++;
    }
    return TRUE;
}
int main() { EnumWindows(cb, 0); printf("contacted %d window(s)\n", sent); return 0; }
