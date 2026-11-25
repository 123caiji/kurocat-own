#pragma once
// ActionHud.hpp —— 操作信息条（自有分支新增）
// 设计目标（P0-V1）：
//   * 常驻不消失：超时只降透明度到 55%，空闲显示「等待输入」，避免"以为没反应"；
//   * 信息分层：键名（主字，强调色）→ 动作词 → 组合键（副字）；
//   * 右侧状态灯：鼠标左/右/中三键 + 滚轮方向；
//   * 零依赖：只用 GDI+ 与 Win32；C++17；不分配堆内存。
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include "KeyMap.hpp"

namespace ActionHud {

// 键名：修饰键/特殊键用中文，其余沿用上游键帽徽标（字母/数字/箭头/F1…）
inline std::wstring keyName(PhysicalKey k) {
    switch (k) {
        case PhysicalKey::Escape: return L"Esc";
        case PhysicalKey::Backspace: return L"退格";
        case PhysicalKey::ForwardDelete: return L"Delete";
        case PhysicalKey::Tab: return L"Tab";
        case PhysicalKey::CapsLock: return L"CapsLock";
        case PhysicalKey::Enter: return L"回车";
        case PhysicalKey::LeftShift: return L"左Shift";
        case PhysicalKey::RightShift: return L"右Shift";
        case PhysicalKey::LeftControl: return L"左Ctrl";
        case PhysicalKey::RightControl: return L"右Ctrl";
        case PhysicalKey::LeftOption: return L"左Alt";
        case PhysicalKey::RightOption: return L"右Alt";
        case PhysicalKey::LeftCommand: return L"左Win";
        case PhysicalKey::RightCommand: return L"右Win";
        case PhysicalKey::Space: return L"空格";
        case PhysicalKey::ArrowUp: return L"↑";
        case PhysicalKey::ArrowDown: return L"↓";
        case PhysicalKey::ArrowLeft: return L"←";
        case PhysicalKey::ArrowRight: return L"→";
        case PhysicalKey::Grave: return L"~";
        case PhysicalKey::Semicolon: return L";";
        case PhysicalKey::Quote: return L"'";
        case PhysicalKey::Comma: return L",";
        case PhysicalKey::Period: return L".";
        case PhysicalKey::Slash: return L"/";
        case PhysicalKey::LeftBracket: return L"[";
        case PhysicalKey::RightBracket: return L"]";
        case PhysicalKey::Minus: return L"-";
        case PhysicalKey::Equal: return L"=";
        default: break;
    }
    std::wstring badge = keyBadgeLabel(k);
    if (badge.empty()) return L"键";
    return badge;
}

namespace detail {
inline bool available(Gdiplus::FontFamily& fam) { return fam.GetLastStatus() == Gdiplus::Ok; }

struct Colors {
    static constexpr BYTE bgA = 0xE2;
    static constexpr BYTE bgR = 0x0F, bgG = 0x12, bgB = 0x19;
    static constexpr BYTE accentR = 0xD4, accentG = 0xF0, accentB = 0x44;
    static constexpr BYTE infoR = 0x38, infoG = 0xBD, infoB = 0xF8;
    static constexpr BYTE textR = 0xE8, textG = 0xED, textB = 0xF5;
    static constexpr BYTE dimR = 0x9A, dimG = 0xA6, dimB = 0xB8;
};
}  // namespace detail

/// 画信息条。age = 距最近一次动作的秒数；wheelDir: +1 上 / -1 下 / 0 无；wheelAge 同上。
inline void draw(Gdiplus::Graphics& g,
                 const std::wstring& name,
                 const std::wstring& verb,
                 const std::wstring& combo,
                 bool mouseLeft, bool mouseRight, bool mouseMiddle,
                 int wheelDir, double wheelAge,
                 double age) {
    const float canvasW = (float)CatPhysics::CanvasWidth;

    // 静态资源：字体/格式只建一次（每帧新建 Font/FontFamily 会吃满一个核）
    struct Res {
        Gdiplus::FontFamily* fam = nullptr;
        Gdiplus::Font* fName = nullptr;
        Gdiplus::Font* fSmall = nullptr;
        Gdiplus::Font* fChip = nullptr;
        Gdiplus::StringFormat* fmt = nullptr;
        Gdiplus::FontFamily famUi{ L"Microsoft YaHei UI" };
        Gdiplus::FontFamily famCn{ L"Microsoft YaHei" };
        Gdiplus::FontFamily famMono{ L"Consolas" };
        Res() {
            fam = detail::available(famUi) ? &famUi : &famCn;
            fName = new Gdiplus::Font(fam, 15.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            fSmall = new Gdiplus::Font(fam, 12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            fChip = new Gdiplus::Font(&famMono, 10.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            fmt = new Gdiplus::StringFormat();
            fmt->SetAlignment(Gdiplus::StringAlignmentCenter);
            fmt->SetLineAlignment(Gdiplus::StringAlignmentCenter);
        }
        ~Res() { delete fName; delete fSmall; delete fChip; delete fmt; }
    };
    static Res res;
    Gdiplus::Font& fName = *res.fName;
    Gdiplus::Font& fSmall = *res.fSmall;
    Gdiplus::Font& fChip = *res.fChip;
    Gdiplus::StringFormat& fmtCenter = *res.fmt;

    // 文本：键名（主） + 动作/组合（副）
    std::wstring sub = verb;
    if (!combo.empty()) {
        if (!sub.empty()) sub += L"  ";
        sub += combo;
    }

    // 宽度缓存：只有文本变化时才 MeasureString
    static std::wstring cName, cSub;
    static float wName = 0.0f, wSub = 0.0f;
    if (name != cName) {
        Gdiplus::RectF rr;
        g.MeasureString(name.c_str(), (INT)name.size(), &fName, Gdiplus::PointF(0, 0), &fmtCenter, &rr);
        wName = rr.Width;
        cName = name;
    }
    if (sub != cSub) {
        Gdiplus::RectF rr;
        g.MeasureString(sub.c_str(), (INT)sub.size(), &fSmall, Gdiplus::PointF(0, 0), &fmtCenter, &rr);
        wSub = rr.Width;
        cSub = sub;
    }
    Gdiplus::RectF rName(0.0f, 0.0f, wName, 0.0f);
    Gdiplus::RectF rSub(0.0f, 0.0f, wSub, 0.0f);

    const float padX = 11.0f, gap = 8.0f, chipW = 15.0f, chipGap = 4.0f, wheelW = 14.0f;
    const float chipsW = chipW * 3.0f + chipGap * 2.0f + (wheelDir != 0 ? (wheelW + chipGap) : 0.0f);
    float w = padX * 2.0f + rName.Width + (sub.empty() ? 0.0f : gap + rSub.Width) + gap + chipsW;
    const float maxW = canvasW - 12.0f;
    if (w > maxW) w = maxW;
    const float h = 27.0f;
    const float x = (canvasW - w) * 0.5f;
    const float y = 5.0f;

    // 常驻：1.65s 后淡到 55%（不消失）
    float alpha = 1.0f;
    if (age > 1.65) alpha = 0.55f;
    else if (age > 1.10) alpha = 1.0f - 0.45f * (float)((age - 1.10) / 0.55);
    const BYTE a = (BYTE)(255.0f * alpha);

    // 圆角底
    Gdiplus::GraphicsPath path;
    const float rad = 9.5f;
    path.AddArc(x, y, rad * 2, rad * 2, 180.0f, 90.0f);
    path.AddArc(x + w - rad * 2, y, rad * 2, rad * 2, 270.0f, 90.0f);
    path.AddArc(x + w - rad * 2, y + h - rad * 2, rad * 2, rad * 2, 0.0f, 90.0f);
    path.AddArc(x, y + h - rad * 2, rad * 2, rad * 2, 90.0f, 90.0f);
    path.CloseFigure();

    Gdiplus::SolidBrush bg(Gdiplus::Color((BYTE)(detail::Colors::bgA * alpha),
                                          detail::Colors::bgR, detail::Colors::bgG, detail::Colors::bgB));
    Gdiplus::Pen border(Gdiplus::Color((BYTE)(70 * alpha), 90, 100, 120), 1.0f);
    g.FillPath(&bg, &path);
    g.DrawPath(&border, &path);

    // 键名（主字）：仅在最近 1.65s 内用强调色，之后转中性文本色
    const bool fresh = age <= 1.65;
    Gdiplus::SolidBrush brName(fresh ? Gdiplus::Color(a, detail::Colors::accentR, detail::Colors::accentG, detail::Colors::accentB)
                                     : Gdiplus::Color(a, detail::Colors::textR, detail::Colors::textG, detail::Colors::textB));
    Gdiplus::SolidBrush brSub(Gdiplus::Color((BYTE)(a * 0.86f), detail::Colors::dimR, detail::Colors::dimG, detail::Colors::dimB));

    float tx = x + padX;
    Gdiplus::RectF rn(tx, y, rName.Width + 2.0f, h);
    g.DrawString(name.c_str(), (INT)name.size(), &fName, rn, &fmtCenter, &brName);
    tx += rName.Width;
    if (!sub.empty()) {
        Gdiplus::RectF rs(tx + gap, y, rSub.Width + 2.0f, h);
        g.DrawString(sub.c_str(), (INT)sub.size(), &fSmall, rs, &fmtCenter, &brSub);
    }

    // 右侧状态灯：L / R / M（按下=信息蓝填充）
    float cx = x + w - padX - chipsW;
    const bool btns[3] = { mouseLeft, mouseRight, mouseMiddle };
    const wchar_t* labels[3] = { L"L", L"R", L"M" };
    for (int i = 0; i < 3; ++i) {
        Gdiplus::RectF chip(cx + i * (chipW + chipGap), y + (h - chipW) * 0.5f, chipW, chipW);
        Gdiplus::GraphicsPath cp;
        cp.AddArc(chip.X, chip.Y, 4.0f, 4.0f, 180.0f, 90.0f);
        cp.AddArc(chip.X + chipW - 4.0f, chip.Y, 4.0f, 4.0f, 270.0f, 90.0f);
        cp.AddArc(chip.X + chipW - 4.0f, chip.Y + chipW - 4.0f, 4.0f, 4.0f, 0.0f, 90.0f);
        cp.AddArc(chip.X, chip.Y + chipW - 4.0f, 4.0f, 4.0f, 90.0f, 90.0f);
        cp.CloseFigure();
        if (btns[i]) {
            Gdiplus::SolidBrush on(Gdiplus::Color((BYTE)(a * 0.95f), detail::Colors::infoR, detail::Colors::infoG, detail::Colors::infoB));
            g.FillPath(&on, &cp);
        } else {
            Gdiplus::Pen offPen(Gdiplus::Color((BYTE)(a * 0.45f), 110, 120, 140), 1.0f);
            g.DrawPath(&offPen, &cp);
        }
        Gdiplus::SolidBrush brChip(btns[i] ? Gdiplus::Color(a, 16, 20, 26)
                                           : Gdiplus::Color((BYTE)(a * 0.6f), detail::Colors::dimR, detail::Colors::dimG, detail::Colors::dimB));
        g.DrawString(labels[i], 1, &fChip, chip, &fmtCenter, &brChip);
    }

    // 滚轮方向（最近 0.6s 内显示）
    if (wheelDir != 0 && wheelAge <= 0.6) {
        Gdiplus::RectF wr(cx + chipsW - wheelW, y, wheelW, h);
        Gdiplus::SolidBrush brWheel(Gdiplus::Color(a, detail::Colors::infoR, detail::Colors::infoG, detail::Colors::infoB));
        g.DrawString(wheelDir > 0 ? L"↑" : L"↓", 1, &fName, wr, &fmtCenter, &brWheel);
    }
}

}  // namespace ActionHud
