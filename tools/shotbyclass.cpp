#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static HWND g_target = nullptr;
static const wchar_t* g_sub = L"";

static BOOL CALLBACK enumTop(HWND h, LPARAM) {
    wchar_t cls[256] = {0};
    GetClassNameW(h, cls, 255);
    if (wcsstr(cls, g_sub) != nullptr) { g_target = h; return FALSE; }
    return TRUE;
}

int main(int argc, char** argv) {
    if (argc < 3) { printf("usage: shotbyclass.exe <classSubstr> <out.bmp>\n"); return 2; }
    static wchar_t sub[128] = {0};
    MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, sub, 127);
    g_sub = sub;
    EnumWindows(enumTop, 0);
    if (!g_target) { printf("class window not found\n"); return 1; }

    RECT rc{}; GetWindowRect(g_target, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) { printf("bad rect\n"); return 1; }

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
    HGDIOBJ old = SelectObject(mem, bmp);

    // 背景先刷成浅色，避免透明处变黑
    HBRUSH bg = CreateSolidBrush(RGB(248, 249, 251));
    RECT all{0, 0, w, h};
    FillRect(mem, &all, bg);
    DeleteObject(bg);

    // WM_PRINT + PRF_CHILDREN：DefWindowProc 会连带打印子控件
    // 置顶后再从屏幕抓（子控件是真实窗口，只有屏幕合成才画得出来）
    BOOL wasTopmost = (GetWindowLongW(g_target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    SetWindowPos(g_target, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    Sleep(400);
    RedrawWindow(g_target, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_ERASE);
    Sleep(200);
    const LPARAM prf = PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CHILDREN | PRF_OWNED | PRF_CHECKVISIBLE;
    SendMessageW(g_target, WM_PRINT, (WPARAM)mem, prf);

    int rowBytes = ((w * 24 + 31) / 32) * 4, dataSize = rowBytes * h, fileSize = 54 + dataSize;
    BYTE* buf = (BYTE*)calloc(1, fileSize);
    buf[0] = 'B'; buf[1] = 'M'; memcpy(buf + 2, &fileSize, 4);
    int off = 54; memcpy(buf + 10, &off, 4);
    int hs = 40; memcpy(buf + 14, &hs, 4);
    memcpy(buf + 18, &w, 4); memcpy(buf + 22, &h, 4);
    short planes = 1, bpp = 24; memcpy(buf + 26, &planes, 2); memcpy(buf + 28, &bpp, 2);
    memcpy(buf + 34, &dataSize, 4);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = h;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 24; bi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(mem, bmp, 0, h, buf + 54, &bi, DIB_RGB_COLORS);

    SelectObject(mem, old);
    DeleteObject(bmp); DeleteDC(mem); ReleaseDC(nullptr, screen);

    FILE* f = fopen(argv[2], "wb");
    if (!f) return 1;
    fwrite(buf, 1, fileSize, f);
    fclose(f);
    free(buf);
    printf("captured %dx%d at %d,%d\n", w, h, (int)rc.left, (int)rc.top);
    return 0;
}
