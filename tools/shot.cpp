#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
int main(int argc, char** argv) {
    if (argc < 3) { printf("usage: shot.exe <title> <out.bmp>\n"); return 2; }
    wchar_t t[256] = {0}; MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, t, 255);
    HWND h = FindWindowW(nullptr, t);
    if (!h) { printf("window not found\n"); return 1; }
    RECT rc{}; GetWindowRect(h, &rc);
    int w = rc.right - rc.left, hh = rc.bottom - rc.top;
    if (w <= 0 || hh <= 0) { printf("bad rect\n"); return 1; }
    HDC s = GetDC(nullptr), m = CreateCompatibleDC(s);
    HBITMAP bmp = CreateCompatibleBitmap(s, w, hh);
    SelectObject(m, bmp);
    BitBlt(m, 0, 0, w, hh, s, rc.left, rc.top, SRCCOPY);
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
    FILE* f = fopen(argv[2], "wb"); if (!f) return 1;
    fwrite(buf, 1, fileSize, f); fclose(f);
    printf("shot: %dx%d at %d,%d\n", w, hh, (int)rc.left, (int)rc.top);
    return 0;
}
