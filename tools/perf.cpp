#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
static DWORD findPid(const wchar_t* name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W e{}; e.dwSize = sizeof(e);
    DWORD pid = 0;
    if (Process32FirstW(snap, &e)) do { if (_wcsicmp(e.szExeFile, name) == 0) { pid = e.th32ProcessID; break; } } while (Process32NextW(snap, &e));
    CloseHandle(snap);
    return pid;
}
int main(int argc, char** argv) {
    if (argc < 2) { printf("usage: perf.exe <processName> [seconds]\n"); return 2; }
    int secs = argc > 2 ? atoi(argv[2]) : 8;
    wchar_t wname[260] = {0};
    MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, wname, 259);
    DWORD pid = findPid(wname);
    if (!pid) { printf("not found\n"); return 1; }
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!h) { printf("open failed %lu\n", GetLastError()); return 1; }
    FILETIME c{}, ex{}, k1{}, u1{};
    GetProcessTimes(h, &c, &ex, &k1, &u1);
    Sleep((DWORD)secs * 1000);
    FILETIME c2{}, ex2{}, k2{}, u2{};
    GetProcessTimes(h, &c2, &ex2, &k2, &u2);
    PROCESS_MEMORY_COUNTERS_EX mc{}; mc.cb = sizeof(mc);
    GetProcessMemoryInfo(h, (PROCESS_MEMORY_COUNTERS*)&mc, sizeof(mc));
    auto f = [](const FILETIME& t) { return ((ULONGLONG)t.dwHighDateTime << 32) | t.dwLowDateTime; };
    double cpu = (double)((f(k2) - f(k1)) + (f(u2) - f(u1))) / ((double)secs * 10000000.0) * 100.0;
    printf("pid=%lu cpu_one_core=%.3f%% rss=%.1fMB private=%.1fMB\n", (unsigned long)pid, cpu,
           mc.WorkingSetSize / 1048576.0, mc.PrivateUsage / 1048576.0);
    CloseHandle(h);
    return 0;
}