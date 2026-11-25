#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
int main(int argc, char** argv) {
    if (argc < 2) { printf("usage: shotfull.exe <out.bmp>\n"); return 2; }
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN), h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN), y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    HDC s = GetDC(nullptr), m = CreateCompatibleDC(s);
    HBITMAP bmp = CreateCompatibleBitmap(s, w, h);
    SelectObject(m, bmp);
    BitBlt(m, 0, 0, w, h, s, x, y, SRCCOPY);
    int rowBytes = ((w * 24 + 31) / 32) * 4, dataSize = rowBytes * h, fileSize = 54 + dataSize;
    BYTE* buf = (BYTE*)calloc(1, fileSize);
    buf[0] = 'B'; buf[1] = 'M'; memcpy(buf + 2, &fileSize, 4);
    int off = 54; memcpy(buf + 10, &off, 4);
    int hs = 40; memcpy(buf + 14, &hs, 4);
    memcpy(buf + 18, &w, 4); memcpy(buf + 22, &h, 4);
    short planes = 1, bpp = 24; memcpy(buf + 26, &planes, 2); memcpy(buf + 28, &bpp, 2);
    memcpy(buf + 34, &dataSize, 4);
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = h; bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24; bi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(m, bmp, 0, h, buf + 54, &bi, DIB_RGB_COLORS);
    FILE* f = fopen(argv[1], "wb"); if (!f) return 1;
    fwrite(buf, 1, fileSize, f); fclose(f);
    printf("full screen %dx%d at %d,%d\n", w, h, x, y);
    return 0;
}
