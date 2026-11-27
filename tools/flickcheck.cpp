#include <windows.h>
#include <cstdio>
#include <vector>
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 40;
    HWND h = FindWindowW(nullptr, L"Tama");
    if (!h) { printf("window not found\n"); return 1; }
    RECT rc{}; GetWindowRect(h, &rc);
    int w = rc.right - rc.left, hh = rc.bottom - rc.top;
    HDC s = GetDC(nullptr), m = CreateCompatibleDC(s);
    HBITMAP bmp = CreateCompatibleBitmap(s, w, hh);
    SelectObject(m, bmp);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -hh;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    std::vector<DWORD> px(w * hh);
    printf("samples:");
    for (int k = 0; k < n; k++) {
        BitBlt(m, 0, 0, w, hh, s, rc.left, rc.top, SRCCOPY);
        GetDIBits(m, bmp, 0, hh, px.data(), &bi, DIB_RGB_COLORS);
        // 以左上角像素为背景，统计"不同"像素比例
        DWORD bg = px[0];
        int diff = 0, tot = 0;
        for (int y = 0; y < hh; y += 3) for (int x = 0; x < w; x += 3) {
            tot++;
            DWORD c = px[y * w + x];
            int dr = abs((int)((c >> 16) & 0xFF) - (int)((bg >> 16) & 0xFF));
            int dg = abs((int)((c >> 8) & 0xFF) - (int)((bg >> 8) & 0xFF));
            int db = abs((int)(c & 0xFF) - (int)(bg & 0xFF));
            if (dr + dg + db > 30) diff++;
        }
        printf(" %.1f", (double)diff / tot * 100.0);
        fflush(stdout);
        Sleep(50);
    }
    printf("\n");
    return 0;
}
