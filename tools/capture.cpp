// capture.cpp — 抓窗口连续帧（可选：移动鼠标让猫转头），输出 frame-000.bmp ...
// usage: capture.exe <title> <outDir> <frames> <intervalMs> [scale]
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

static void saveBmpAt(HWND h, const std::wstring& path) {
    RECT rc{}; GetWindowRect(h, &rc);
    int w = rc.right - rc.left, hh = rc.bottom - rc.top;
    if (w <= 0 || hh <= 0) return;
    HDC s = GetDC(nullptr), m = CreateCompatibleDC(s);
    HBITMAP bmp = CreateCompatibleBitmap(s, w, hh);
    SelectObject(m, bmp);
    BOOL ok = PrintWindow(h, m, 2 /*PW_RENDERFULLCONTENT*/);
    if (!ok) BitBlt(m, 0, 0, w, hh, s, rc.left, rc.top, SRCCOPY);
    int rowBytes = ((w * 24 + 31) / 32) * 4, dataSize = rowBytes * hh, fileSize = 54 + dataSize;
    BYTE* buf = (BYTE*)calloc(1, fileSize);
    buf[0] = 'B'; buf[1] = 'M'; memcpy(buf + 2, &fileSize, 4);
    int off = 54; memcpy(buf + 10, &off, 4);
    int hs = 40; memcpy(buf + 14, &hs, 4);
    memcpy(buf + 18, &w, 4); memcpy(buf + 22, &hh, 4);
    short planes = 1, bpp = 24; memcpy(buf + 26, &planes, 2); memcpy(buf + 28, &bpp, 2);
    memcpy(buf + 34, &dataSize, 4);
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = hh;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 24; bi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(m, bmp, 0, hh, buf + 54, &bi, DIB_RGB_COLORS);
    HANDLE hf = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(hf, buf, (DWORD)fileSize, &written, nullptr);
        CloseHandle(hf);
        if (written != (DWORD)fileSize) printf("write short: %lu/%d\n", written, fileSize);
    } else {
        printf("CreateFile failed err=%lu\n", GetLastError());
    }
    free(buf);
    SelectObject(m, bmp); DeleteObject(bmp); DeleteDC(m); ReleaseDC(nullptr, s);
}

int main(int argc, char** argv) {
    if (argc < 5) { printf("usage: capture.exe <title> <outDir> <frames> <intervalMs>\n"); return 2; }
    wchar_t t[256] = {0}; MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, t, 255);
    wchar_t odBuf[512] = {0};
    MultiByteToWideChar(CP_UTF8, 0, argv[2], -1, odBuf, 511);
    std::wstring outDir(odBuf);
    int frames = atoi(argv[3]), interval = atoi(argv[4]);
    HWND h = FindWindowW(nullptr, t);
    if (!h) { printf("window not found\n"); return 1; }

    POINT saved{}; GetCursorPos(&saved);
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    for (int i = 0; i < frames; i++) {
        double a = (double)i / (double)frames * 6.2831853 * 2.0;
        SetCursorPos(sw / 2 + (int)(cos(a) * sw * 0.22), sh / 2 + (int)(sin(a) * sh * 0.18));
        wchar_t num[16] = {0};
        swprintf(num, 16, L"%03d", i);
        std::wstring name = outDir + L"\\frame-" + std::wstring(num) + L".bmp";
        saveBmpAt(h, name);
        Sleep(interval);
    }
    SetCursorPos(saved.x, saved.y);
    printf("captured %d frames to %s\n", frames, argv[2]);
    return 0;
}
