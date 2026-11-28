#pragma once
// =====================================================================================
// PoseTune.hpp —— 桌宠四足姿态「拖动式」实时调参面板（v0.12.2）
//
// 目的：不再靠"改代码 → 重新编译 → 抓帧肉眼比对"来调四足姿态。
//       --pose-tune 启动后，四足姿态被强制打开，左上角出现参数面板，
//       鼠标（按住窗口任一位置）横向拖动即可实时改参数、立刻看到画面变化。
//
// 用法：
//   dist\tama_walkdemo.exe --pose-tune
//   · 拖动某一行的数值 → 该参数实时增减（面板内单击选中行，横向拖 = 调值）
//   · 点最后一行 RESET → 全部恢复默认值
//   · 参数一改动就写 exe 同级 pose_tune_params.txt，内容是可直接粘回源码的 C++ 常量
// =====================================================================================
#include <windows.h>
#include <gdiplus.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include "CatPhysics.hpp"

namespace PoseTune {

struct Row {
    const char* name;   // 显示名
    float*      p;      // 指向 CatPhysics 的运行时参数
    float       step;   // 每 1 画布像素拖动的增量
    float       lo;
    float       hi;
    float       def;    // 默认（定稿）值
    const char* cpp;    // 导出用的源码名
};

inline Row* table() {
    static Row r[] = {
        { "SeatDY",   &CatPhysics::WalkSeatDY,     0.25f,  -60.0f,  0.0f, -26.0f, "WalkSeatDY"     },
        { "GroundY",  &CatPhysics::WalkGroundY,    0.20f,  235.0f, 275.0f, 250.0f, "WalkGroundY"    },
        { "ShoulderDY",&CatPhysics::WalkShoulderDY, 0.20f,  8.0f,   40.0f,  20.0f, "WalkShoulderDY" },
        { "HipDY",    &CatPhysics::WalkHipDY,      0.20f,   8.0f,   40.0f,  22.0f, "WalkHipDY"      },
        { "ReachF",   &CatPhysics::WalkFootReachF, 0.20f,   2.0f,   30.0f,  15.0f, "WalkFootReachF" },
        { "ReachH",   &CatPhysics::WalkFootReachH, 0.20f,   2.0f,   30.0f,  13.0f, "WalkFootReachH" },
        { "HeadDY",   &CatPhysics::WalkHeadDY,     0.25f,  -45.0f,  0.0f,  -20.0f, "WalkHeadDY"     },
        { "FreqBase", &CatPhysics::GaitFreqBase,   0.01f,   0.80f,  2.20f,  1.10f, "GaitFreqBase"   },
        { "FreqMin",  &CatPhysics::GaitFreqMin,    0.01f,   1.00f,  2.60f,  1.55f, "GaitFreqMin"    },
        { "FreqMax",  &CatPhysics::GaitFreqMax,    0.01f,   1.20f,  3.20f,  2.05f, "GaitFreqMax"    },
        { "Duty",     &CatPhysics::GaitDuty,       0.005f,  0.45f,  0.80f,  0.62f, "GaitDuty"       },
        { "Stride",   &CatPhysics::GaitStrideGain, 0.01f,   0.70f,  1.80f,  1.15f, "GaitStrideGain" },
        { "LiftF",    &CatPhysics::GaitLiftF,      0.10f,   2.0f,   16.0f,  7.0f,  "GaitLiftF"      },
        { "LiftH",    &CatPhysics::GaitLiftH,      0.10f,   2.0f,   16.0f,  6.0f,  "GaitLiftH"      },
        { "HindOutK", &CatPhysics::HindOutK,       0.005f,  0.0f,   0.95f,  0.52f, "HindOutK"       },
        { "Splay",    &CatPhysics::HindFootSplay,  0.20f,   0.0f,   32.0f,  20.0f, "HindFootSplay"  },
    };
    return r;
}

inline int  rowCount() { return 16; }
inline bool& enabled() { static bool v = false; return v; }
inline int&  sel()     { static int  v = 0;     return v; }
inline bool& dragging(){ static bool v = false; return v; }
inline POINT& lastPt() { static POINT p = { 0, 0 }; return p; }
inline float& lastCy() { static float v = 0.0f; return v; }
inline int&  resetHitPulse() { static int v = 0; return v; }

// ---- 面板布局（画布坐标；绘制时 g 已含画布缩放） ----
// 两列紧凑排布，高度压到 ~155px，避免遮住画布中下部的猫身
constexpr float PX = 6.0f;
constexpr float PY = 6.0f;
constexpr float PW = 268.0f;
constexpr float COLW = 134.0f;
constexpr float RH = 15.0f;
constexpr int   ROWS_PER_COL = 8;
inline float headerH()  { return 32.0f; }
inline float panelH()   { return headerH() + (ROWS_PER_COL + 1) * RH + 6.0f; }
inline float rowTop(int rowInCol) { return PY + headerH() + rowInCol * RH; }

// 命中检测：>=0 行号，rowCount() 表示 RESET 行，-1 面板内空白，-2 面板外
inline int hitTest(float cx, float cy) {
    if (cx < PX || cx > PX + PW || cy < PY || cy > PY + panelH()) return -2;
    float rel = (cy - PY - headerH()) / RH;
    if (rel < 0.0f) return -1;
    int row = (int)rel;
    if (row > ROWS_PER_COL) return -1;
    if (row == ROWS_PER_COL) return rowCount();            // RESET 行（跨两列）
    float relx = cx - PX;
    int col = (relx < COLW) ? 0 : 1;
    if (relx > COLW * 2.0f) return -1;
    int idx = col * ROWS_PER_COL + row;
    return (idx < rowCount()) ? idx : -1;
}

// 拖动改值：dxCanvas 为画布像素增量
inline void dragBy(int idx, float dxCanvas) {
    if (idx < 0 || idx >= rowCount()) return;
    Row* r = table();
    float v = *r[idx].p + dxCanvas * r[idx].step;
    if (v < r[idx].lo) v = r[idx].lo;
    if (v > r[idx].hi) v = r[idx].hi;
    *r[idx].p = v;
    sel() = idx;
}

inline void resetAll() {
    Row* r = table();
    for (int i = 0; i < rowCount(); ++i) *r[i].p = r[i].def;
}

// 导出为可粘回源码的常量片段
inline void writeExport() {
    Row* r = table();
    FILE* f = fopen("pose_tune_params.txt", "w");
    if (!f) return;
    fprintf(f, "// kurocat-own 四足姿态调参结果（--pose-tune 拖动生成）\n");
    for (int i = 0; i < rowCount(); ++i) {
        fprintf(f, "static inline float %-15s = %.3ff;   // default %.3f\n",
                r[i].cpp, *r[i].p, r[i].def);
    }
    fclose(f);
}

inline void drawPanel(Gdiplus::Graphics& g) {
    if (!enabled()) return;
    using namespace Gdiplus;
    Row* r = table();

    SolidBrush bg(Color(206, 12, 14, 20));
    Pen border(Color(255, 96, 200, 255), 1.0f);
    g.FillRectangle(&bg, PX, PY, PW, panelH());
    g.DrawRectangle(&border, PX, PY, PW, panelH());

    Font font(L"Consolas", 9.0f, FontStyleRegular, UnitPixel);
    Font fontB(L"Consolas", 9.0f, FontStyleBold, UnitPixel);
    SolidBrush fg(Color(255, 214, 226, 238));
    SolidBrush dim(Color(255, 130, 145, 160));
    SolidBrush hi(Color(255, 255, 214, 102));
    SolidBrush val(Color(255, 122, 232, 190));
    SolidBrush barBg(Color(255, 40, 46, 56));
    SolidBrush barFg(Color(255, 76, 160, 232));
    SolidBrush barHi(Color(255, 255, 190, 84));
    SolidBrush rowHi(Color(64, 120, 190, 255));

    wchar_t head[160];
    swprintf(head, 160, L"POSE TUNE   hold+drag a row  ->  value changes live");
    g.DrawString(head, -1, &fontB, PointF(PX + 6.0f, PY + 3.0f), &hi);
    swprintf(head, 160, L"RESET row = defaults;  saved to pose_tune_params.txt");
    g.DrawString(head, -1, &font, PointF(PX + 6.0f, PY + 17.0f), &dim);

    for (int i = 0; i < rowCount(); ++i) {
        int col = i / ROWS_PER_COL;
        int row = i % ROWS_PER_COL;
        float x0 = PX + col * COLW + 4.0f;
        float y  = rowTop(row);
        bool  s  = (i == sel());
        if (s) g.FillRectangle(&rowHi, PX + col * COLW + 1.0f, y - 1.0f, COLW - 2.0f, RH);

        wchar_t wname[32];
        MultiByteToWideChar(CP_UTF8, 0, r[i].name, -1, wname, 32);
        g.DrawString(wname, -1, s ? &fontB : &font, PointF(x0, y), s ? &hi : &fg);

        wchar_t wv[32];
        swprintf(wv, 32, L"%6.2f", *r[i].p);
        g.DrawString(wv, -1, s ? &fontB : &font, PointF(x0 + 58.0f, y), s ? &val : &fg);

        // 比例条：当前值在 [lo,hi] 中的位置
        float t = (*r[i].p - r[i].lo) / (r[i].hi - r[i].lo);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        g.FillRectangle(&barBg, x0 + 96.0f, y + 4.0f, 34.0f, 6.0f);
        g.FillRectangle(s ? &barHi : &barFg, x0 + 96.0f, y + 4.0f, 34.0f * t, 6.0f);
    }

    float ry = rowTop(ROWS_PER_COL);
    g.DrawString(L"[ RESET ALL TO DEFAULT ]", -1, &fontB, PointF(PX + 6.0f, ry),
                 (resetHitPulse() > 0) ? &hi : &dim);
    if (resetHitPulse() > 0) --resetHitPulse();
}

inline float& lastCx()   { static float v = 0.0f; return v; }
inline bool&  haveLast() { static bool  v = false; return v; }

// 鼠标按下：返回 true 表示事件已被面板吃掉
inline bool onDown(float cx, float cy) {
    if (!enabled()) return false;
    int idx = hitTest(cx, cy);
    if (idx == -2) return false;
    if (idx >= 0 && idx < rowCount()) { sel() = idx; dragging() = true; lastCx() = cx; haveLast() = true; }
    if (idx == rowCount()) { resetAll(); resetHitPulse() = 20; writeExport(); }
    return true;
}

inline void onMove(float cx, float cy) {
    if (!enabled() || !dragging()) return;
    if (!haveLast()) { lastCx() = cx; lastCy() = cy; haveLast() = true; return; }
    float dx = cx - lastCx();
    float dy = cy - lastCy();
    lastCx() = cx;
    lastCy() = cy;
    // 水平拖动最直观；垂直拖动同样可用（向下拖 = 增大）
    float d = (fabsf(dx) >= fabsf(dy)) ? dx : -dy;
    if (d != 0.0f) { dragBy(sel(), d); writeExport(); }
}

inline void onUp() {
    dragging() = false;
    haveLast() = false;
}

} // namespace PoseTune
