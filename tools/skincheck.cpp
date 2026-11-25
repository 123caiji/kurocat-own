// skincheck.cpp — 用运行时同一套 SkinSpec 解析器校验 skins/*.kurocat
#include <windows.h>
#include <cstdio>
#include <string>
#include <vector>
#include "SkinSpec.hpp"

static bool readAll(const std::wstring& p, std::string& out) {
    HANDLE h = CreateFileW(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{}; GetFileSizeEx(h, &sz);
    out.assign((size_t)sz.QuadPart, '\0');
    DWORD rd = 0; BOOL ok = out.empty() || ReadFile(h, &out[0], (DWORD)out.size(), &rd, nullptr);
    CloseHandle(h);
    return ok && rd == out.size();
}

int main(int argc, char** argv) {
    const char* dir = argc > 1 ? argv[1] : "skins";
    wchar_t wdir[MAX_PATH] = {0};
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, MAX_PATH);
    std::wstring pattern = std::wstring(wdir) + L"\\*.kurocat";
    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) { printf("no skins in %s\n", dir); return 1; }
    int okCount = 0, badCount = 0;
    do {
        std::wstring path = std::wstring(wdir) + L"\\" + fd.cFileName;
        std::string bytes;
        if (!readAll(path, bytes)) { printf("READ FAIL %ls\n", fd.cFileName); badCount++; continue; }
        SkinSpec spec;
        std::string err;
        if (SkinSpec::parse(bytes, spec, err)) {
            printf("OK   %-24ls  id=%s  marks=%zu  regions=%zu\n", fd.cFileName, spec.id.c_str(), spec.marks.size(), spec.regions.size());
            okCount++;
        } else {
            printf("FAIL %-24ls  %s\n", fd.cFileName, err.c_str());
            badCount++;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    printf("\n%d ok, %d failed\n", okCount, badCount);
    return badCount == 0 ? 0 : 1;
}
