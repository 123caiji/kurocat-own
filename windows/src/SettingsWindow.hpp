#pragma once
// SettingsWindow.hpp \u2014\u2014 Tama \u539F\u751F\u8BBE\u7F6E\u7A97\u53E3\uFF08\u81EA\u6709 UI\uFF0CWin32 \u539F\u751F\u63A7\u4EF6\uFF0C\u96F6\u4F9D\u8D56\uFF09
// \u8BBE\u8BA1\u76EE\u6807\uFF1A\u628A\u53F3\u952E\u83DC\u5355\u91CC\u7684\u957F\u5217\u8868\u642C\u8FDB\u4E00\u4E2A 560x520 \u7684\u7A97\u53E3\uFF0C\u5206\u7EC4 + \u4E0B\u62C9 + \u52FE\u9009\uFF0C\u6539\u5B8C\u7ACB\u5373\u751F\u6548\u3002
// \u5B9E\u73B0\u65B9\u5F0F\uFF1A\u63A7\u4EF6\u4E8B\u4EF6\u76F4\u63A5\u8C03\u7528 applyAppCommand(app, CMD_*)\uFF0C\u4E0E\u83DC\u5355\u590D\u7528\u540C\u4E00\u5957\u547D\u4EE4\u5904\u7406\u903B\u8F91\u3002

namespace tama_settings_ui {

inline constexpr const wchar_t* kSettingsClass = L"TamaSettingsWindowClass";
inline constexpr int kDesignW = 560;
inline constexpr int kDesignH = 520;

enum Ids : int {
    ID_COAT = 1001,
    ID_EYE,
    ID_KB,
    ID_SCALE,
    ID_LANG,
    ID_IMPORT = 1010,
    ID_WARDROBE,
    ID_BADGES = 1020,
    ID_LEGENDS,
    ID_MARQUEE,
    ID_AUTOHIDE,
    ID_TOPMOST,
    ID_CLICKTHROUGH,
    ID_AUTOSTART,
    ID_SKINDIR = 1040,
    ID_UPDATE,
    ID_DONE
};

struct UiContext {
    AppState* app = nullptr;
    HFONT font = nullptr;
    HFONT boldFont = nullptr;
    HBRUSH bg = nullptr;
};
inline UiContext g_ui;

inline CatCoatTheme coatOfIndex(int i) {
    switch (i) {
        case 0: return CatCoatTheme::ClassicBlack;
        case 1: return CatCoatTheme::Siamese;
        case 2: return CatCoatTheme::Shorthair;
        case 3: return CatCoatTheme::OrangeWhite;
        case 4: return CatCoatTheme::Tabby;
        default: return CatCoatTheme::Dazed;
    }
}
inline UINT coatCommandOfIndex(int i) {
    switch (i) {
        case 0: return CMD_COAT_BLACK;
        case 1: return CMD_COAT_SIAMESE;
        case 2: return CMD_COAT_SHORTHAIR;
        case 3: return CMD_COAT_ORANGE_WHITE;
        case 4: return CMD_COAT_TABBY;
        default: return CMD_COAT_DAZED;
    }
}
inline UINT eyeCommandOfIndex(int i) {
    static const UINT cmds[] = { CMD_EYE_LIME, CMD_EYE_BLUE, CMD_EYE_AMBER, CMD_EYE_CYAN,
                                 CMD_EYE_SOFT_AMBER, CMD_EYE_REFERENCE_HAZEL, CMD_EYE_OLIVE_GOLD };
    if (i < 0 || i >= 7) return CMD_EYE_LIME;
    return cmds[i];
}
inline UINT kbCommandOfIndex(int i) {
    static const UINT cmds[] = { CMD_KB_MILK, CMD_KB_HACKER, CMD_KB_RETRO };
    if (i < 0 || i >= 3) return CMD_KB_MILK;
    return cmds[i];
}
inline UINT scaleCommandOfIndex(int i) {
    static const UINT cmds[] = { CMD_SCALE_65, CMD_SCALE_85, CMD_SCALE_100, CMD_SCALE_125, CMD_SCALE_150 };
    if (i < 0 || i >= 5) return CMD_SCALE_100;
    return cmds[i];
}

inline void setFont(HWND c, HFONT f) { if (c && f) SendMessageW(c, WM_SETFONT, (WPARAM)f, TRUE); }

inline HWND addChild(HWND parent, const wchar_t* cls, const wchar_t* text, DWORD style,
                     int x, int y, int w, int h, int id, float k, bool bold = false) {
    HWND c = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                             (int)(x * k), (int)(y * k), (int)(w * k), (int)(h * k),
                             parent, (HMENU)(INT_PTR)id, GetModuleHandleW(nullptr), nullptr);
    setFont(c, bold ? g_ui.boldFont : g_ui.font);
    return c;
}

inline void addCombo(HWND parent, int x, int y, int w, int id, float k, const std::vector<std::wstring>& items, int sel) {
    HWND c = addChild(parent, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, x, y, w, 200, id, k);
    for (const auto& it : items) SendMessageW(c, CB_ADDSTRING, 0, (LPARAM)it.c_str());
    SendMessageW(c, CB_SETCURSEL, sel < 0 ? 0 : sel, 0);
}

inline void refresh(HWND win) {
    AppState* app = g_ui.app;
    if (!app) return;
    // \u6BDB\u8272\u4E0B\u62C9\u7684\u5F53\u524D\u9879
    if (HWND c = GetDlgItem(win, ID_COAT)) {
        int sel = 5;
        if (!app->settings.customSkin.empty()) {
            for (size_t i = 0; i < app->skins.skins.size(); ++i) {
                if (app->skins.skins[i].id == app->settings.customSkin) { sel = 6 + (int)i; break; }
            }
        } else {
            switch (app->settings.coatTheme) {
                case CatCoatTheme::ClassicBlack: sel = 0; break;
                case CatCoatTheme::Siamese: sel = 1; break;
                case CatCoatTheme::Shorthair: sel = 2; break;
                case CatCoatTheme::OrangeWhite: sel = 3; break;
                case CatCoatTheme::Tabby: sel = 4; break;
                default: sel = 5; break;
            }
        }
        SendMessageW(c, CB_SETCURSEL, sel, 0);
    }
    if (HWND c = GetDlgItem(win, ID_LANG)) SendMessageW(c, CB_SETCURSEL, app->settings.isEnglish ? 1 : 0, 0);
    if (HWND c = GetDlgItem(win, ID_SCALE)) {
        int idx = 2;
        if (app->settings.scale <= 0.70f) idx = 0;
        else if (app->settings.scale <= 0.90f) idx = 1;
        else if (app->settings.scale <= 1.10f) idx = 2;
        else if (app->settings.scale <= 1.35f) idx = 3;
        else idx = 4;
        SendMessageW(c, CB_SETCURSEL, idx, 0);
    }
    auto check = [&](int id, bool on) {
        if (HWND c = GetDlgItem(win, id)) SendMessageW(c, BM_SETCHECK, on ? BST_CHECKED : BST_UNCHECKED, 0);
    };
    check(ID_BADGES, app->settings.showKeyBadges);
    check(ID_LEGENDS, app->settings.showKeyLegends);
    check(ID_MARQUEE, app->settings.keyboardMarquee);
    check(ID_AUTOHIDE, app->settings.autoHideFullscreen);
    check(ID_TOPMOST, app->settings.alwaysOnTop);
    check(ID_CLICKTHROUGH, app->settings.fullClickThrough);
    check(ID_AUTOSTART, isAutoStartEnabled());
}

inline void createWindow(AppState& app);
inline void dbgLog(const char* msg);

inline LRESULT CALLBACK settingsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AppState* app = g_ui.app;
    switch (msg) {
        case WM_COMMAND: {
            if (!app) return 0;
            const int id = LOWORD(wParam);
            const int code = HIWORD(wParam);
            if (code == CBN_SELCHANGE) {
                HWND c = GetDlgItem(hwnd, id);
                int sel = c ? (int)SendMessageW(c, CB_GETCURSEL, 0, 0) : 0;
                if (id == ID_COAT) {
                    if (sel >= 6) {
                        size_t idx = (size_t)(sel - 6);
                        if (idx < app->skins.skins.size()) applyAppCommand(*app, CMD_SKIN_SELECT_BASE + (UINT)idx);
                    } else {
                        applyAppCommand(*app, coatCommandOfIndex(sel));
                    }
                } else if (id == ID_EYE) {
                    applyAppCommand(*app, eyeCommandOfIndex(sel));
                } else if (id == ID_KB) {
                    applyAppCommand(*app, kbCommandOfIndex(sel));
                } else if (id == ID_SCALE) {
                    applyAppCommand(*app, scaleCommandOfIndex(sel));
                } else if (id == ID_LANG) {
                    applyAppCommand(*app, sel == 1 ? CMD_LANG_EN : CMD_LANG_ZH);
                }
                refresh(hwnd);
                return 0;
            }
            if (code == BN_CLICKED) {
                switch (id) {
                    case ID_BADGES:        applyAppCommand(*app, CMD_TOGGLE_BADGES); break;
                    case ID_LEGENDS:       applyAppCommand(*app, CMD_TOGGLE_KEY_LEGENDS); break;
                    case ID_MARQUEE:       applyAppCommand(*app, CMD_TOGGLE_MARQUEE); break;
                    case ID_AUTOHIDE:      applyAppCommand(*app, CMD_TOGGLE_AUTOHIDE_FS); break;
                    case ID_TOPMOST:       applyAppCommand(*app, CMD_TOGGLE_TOPMOST); break;
                    case ID_CLICKTHROUGH:  applyAppCommand(*app, CMD_TOGGLE_CLICKTHROUGH); break;
                    case ID_AUTOSTART:     applyAppCommand(*app, CMD_TOGGLE_AUTOSTART); break;
                    case ID_SKINDIR:       applyAppCommand(*app, CMD_OPEN_SKINS_DIR); break;
                    case ID_WARDROBE:      applyAppCommand(*app, CMD_IMPORT_SKIN); return 0;
                    case ID_UPDATE:        applyAppCommand(*app, CMD_CHECK_UPDATE); return 0;
                    case ID_DONE:          DestroyWindow(hwnd); return 0;
                    default: break;
                }
                refresh(hwnd);
                return 0;
            }
            return 0;
        }
        case WM_CTLCOLORSTATIC: {
            HDC dc = (HDC)wParam;
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(34, 38, 46));
            return (LRESULT)g_ui.bg;
        }
        case WM_ERASEBKGND: {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, g_ui.bg);
            return 1;
        }
        case WM_CLOSE:
            dbgLog("WM_CLOSE received");
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            dbgLog("WM_DESTROY received");
            if (app) app->settingsWnd = nullptr;
            return 0;
        default: break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

inline void dbgLog(const char* msg) {
    if (!getenv("KC_LOG")) return;
    FILE* f = fopen("D:\\kaifa\\work\\kurocat-own\\dist\\settings-debug.log", "a");
    if (f) { fprintf(f, "%s (err=%lu)\n", msg, GetLastError()); fclose(f); }
}

inline void createWindow(AppState& app) {
    dbgLog("createWindow enter");
    HINSTANCE inst = GetModuleHandleW(nullptr);
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = settingsProc;
        wc.hInstance = inst;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = kSettingsClass;
        registered = RegisterClassExW(&wc) != 0;
    }

    HDC screen = GetDC(nullptr);
    float k = GetDeviceCaps(screen, LOGPIXELSY) / 96.0f;
    ReleaseDC(nullptr, screen);

    g_ui.app = &app;
    g_ui.bg = CreateSolidBrush(RGB(248, 249, 251));
    g_ui.font = CreateFontW(-(int)(13 * k), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");
    g_ui.boldFont = CreateFontW(-(int)(13 * k), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");

    bool en = app.settings.isEnglish;
    auto tr = [&](const wchar_t* zh, const wchar_t* eng) -> const wchar_t* { return en ? eng : zh; };

    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    RECT rc = { 0, 0, (LONG)(kDesignW * k), (LONG)(kDesignH * k) };
    AdjustWindowRectEx(&rc, style, FALSE, 0);
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    HWND win = CreateWindowExW(0, kSettingsClass, tr(L"Tama \u8BBE\u7F6E", L"Tama Settings"), style,
                               (work.left + work.right - w) / 2, (work.top + work.bottom - h) / 2,
                               w, h, nullptr, nullptr, inst, nullptr);
    if (!win) { dbgLog("CreateWindowExW failed"); return; }
    dbgLog("settings window created");
    app.settingsWnd = win;

    // ---- \u7B2C 1 \u7EC4\uFF1A\u5916\u89C2 ----
    addChild(win, L"BUTTON", tr(L"\u5916\u89C2", L"Appearance"), BS_GROUPBOX, 14, 10, 532, 150, 0, k, true);
    addChild(win, L"STATIC", tr(L"\u732B\u54AA\u76AE\u80A4", L"Cat skin"), SS_LEFT, 28, 36, 72, 20, 0, k);
    addChild(win, L"STATIC", tr(L"\u77B3\u8272", L"Eye color"), SS_LEFT, 28, 70, 72, 20, 0, k);
    addChild(win, L"STATIC", tr(L"\u952E\u76D8\u914D\u8272", L"Keyboard"), SS_LEFT, 28, 104, 72, 20, 0, k);
    addChild(win, L"STATIC", tr(L"\u7F29\u653E", L"Scale"), SS_LEFT, 28, 138 - 4, 72, 20, 0, k);

    std::vector<std::wstring> coatItems = {
        tr(L"\u7ECF\u5178\u5C0F\u9ED1\u732B", L"Classic black"), tr(L"\u6316\u7164\u66B9\u7F57\u732B", L"Siamese"), tr(L"\u7F8E\u77ED", L"Silver tabby"),
        tr(L"\u6A58\u767D\u732B", L"Orange & white"), tr(L"\u72F8\u82B1\u732B", L"Tabby"), tr(L"\u5446\u732B", L"Dazed")
    };
    for (const auto& s : app.skins.skins) coatItems.push_back(utf8ToWide(s.name));
    addCombo(win, 104, 32, 300, ID_COAT, k, coatItems, 0);
    addCombo(win, 104, 66, 300, ID_EYE, k, { tr(L"\u8367\u5149\u9EC4\u7EFF", L"Lime"), tr(L"\u5B9D\u77F3\u84DD", L"Blue"), tr(L"\u7425\u73C0", L"Amber"),
                                             tr(L"\u51B0\u9752", L"Cyan"), tr(L"\u6696\u7425\u73C0", L"Soft amber"), tr(L"\u699B\u679C", L"Hazel"), tr(L"\u6A44\u6984\u91D1", L"Olive gold") }, 0);
    addCombo(win, 104, 100, 300, ID_KB, k, { tr(L"\u7ECF\u5178\u5976\u767D", L"Classic milk"), tr(L"\u5348\u591C\u6781\u5BA2", L"Midnight hacker"), tr(L"\u590D\u53E4\u5976\u6CB9", L"Retro cream") }, 0);
    addCombo(win, 104, 134 - 4, 120, ID_SCALE, k, { L"65%", L"85%", L"100%", L"125%", L"150%" }, 2);
    addChild(win, L"BUTTON", tr(L"\u6362\u88C5\u95F4\u2026", L"Wardrobe\u2026"), BS_PUSHBUTTON | WS_TABSTOP, 420, 66, 110, 24, ID_WARDROBE, k);

    // ---- \u7B2C 2 \u7EC4\uFF1A\u884C\u4E3A\u4E0E\u53CD\u9988 ----
    addChild(win, L"BUTTON", tr(L"\u884C\u4E3A\u4E0E\u53CD\u9988", L"Behavior"), BS_GROUPBOX, 14, 170, 532, 128, 0, k, true);
    addChild(win, L"BUTTON", tr(L"\u663E\u793A\u952E\u5E3D\u6C14\u6CE1", L"Keycap bubbles"), BS_AUTOCHECKBOX | WS_TABSTOP, 28, 196, 200, 22, ID_BADGES, k);
    addChild(win, L"BUTTON", tr(L"\u663E\u793A\u952E\u4F4D\u56FE\u4F8B", L"Key legends"), BS_AUTOCHECKBOX | WS_TABSTOP, 250, 196, 200, 22, ID_LEGENDS, k);
    addChild(win, L"BUTTON", tr(L"\u952E\u76D8\u8DD1\u9A6C\u706F", L"Keyboard marquee"), BS_AUTOCHECKBOX | WS_TABSTOP, 28, 224, 200, 22, ID_MARQUEE, k);
    addChild(win, L"BUTTON", tr(L"\u5168\u5C4F\u65F6\u81EA\u52A8\u9690\u85CF", L"Hide on fullscreen"), BS_AUTOCHECKBOX | WS_TABSTOP, 250, 224, 220, 22, ID_AUTOHIDE, k);
    addChild(win, L"BUTTON", tr(L"\u7A97\u53E3\u7F6E\u9876", L"Always on top"), BS_AUTOCHECKBOX | WS_TABSTOP, 28, 252, 200, 22, ID_TOPMOST, k);
    addChild(win, L"BUTTON", tr(L"\u9F20\u6807\u7A7F\u900F", L"Click-through"), BS_AUTOCHECKBOX | WS_TABSTOP, 250, 252, 220, 22, ID_CLICKTHROUGH, k);

    // ---- \u7B2C 3 \u7EC4\uFF1A\u7CFB\u7EDF ----
    addChild(win, L"BUTTON", tr(L"\u7CFB\u7EDF", L"System"), BS_GROUPBOX, 14, 310, 532, 96, 0, k, true);
    addChild(win, L"BUTTON", tr(L"\u5F00\u673A\u81EA\u542F", L"Run at startup"), BS_AUTOCHECKBOX | WS_TABSTOP, 28, 336, 200, 22, ID_AUTOSTART, k);
    addChild(win, L"STATIC", tr(L"\u8BED\u8A00", L"Language"), SS_LEFT, 28, 370, 72, 20, 0, k);
    addCombo(win, 104, 366, 140, ID_LANG, k, { L"\u7B80\u4F53\u4E2D\u6587", L"English" }, 0);
    addChild(win, L"BUTTON", tr(L"\u6253\u5F00\u76AE\u80A4\u6587\u4EF6\u5939", L"Open skins folder"), BS_PUSHBUTTON | WS_TABSTOP, 300, 366, 120, 24, ID_SKINDIR, k);
    addChild(win, L"BUTTON", tr(L"\u68C0\u67E5\u66F4\u65B0", L"Check updates"), BS_PUSHBUTTON | WS_TABSTOP, 426, 366, 104, 24, ID_UPDATE, k);

    // ---- \u5E95\u90E8 ----
    wchar_t about[160] = {};
    swprintf_s(about, L"%ls", tr(L"Tama v", L"Tama v"));
    std::wstring aboutLine = std::wstring(about) + APP_VERSION +
        L"  \u00B7  " + tr(L"\u4E0A\u6E38\u81F4\u8C22 chanke17/KuroCat (MIT)", L"Upstream credit: chanke17/KuroCat (MIT)");
    addChild(win, L"STATIC", aboutLine.c_str(), SS_LEFT, 20, 430, 380, 20, 0, k);
    addChild(win, L"BUTTON", tr(L"\u5B8C\u6210", L"Done"), BS_DEFPUSHBUTTON | WS_TABSTOP, 450, 424, 96, 30, ID_DONE, k);

    refresh(win);
    ShowWindow(win, SW_SHOWNORMAL);
    SetForegroundWindow(win);
}

inline void showSettingsWindow(AppState& app) {
    if (app.settingsWnd && IsWindow(app.settingsWnd)) {
        refresh(app.settingsWnd);
        ShowWindow(app.settingsWnd, SW_SHOWNORMAL);
        SetForegroundWindow(app.settingsWnd);
        return;
    }
    createWindow(app);
}

}  // namespace tama_settings_ui

inline void showSettingsWindow(AppState& app) { tama_settings_ui::showSettingsWindow(app); }
