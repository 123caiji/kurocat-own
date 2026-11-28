// CatSidePose.hpp —— 侧身四足站立 / 小跑渲染（纯剪影式，对齐用户参考图）
// 参考图量测（446x397，猫轮廓 384x179）：
//   躯干：扁平长身，长高比约 3.2:1
//   头：小圆头与背线齐平、无颈，直径约为躯干的 1/4
//   耳：尖立小三角，两耳间距约 1 个头半径，略外扩
//   眼：黄绿小圆眼，位于头部中上部，两眼中距约 1.4 倍眼径
//   腿：四条直立细腿，等长垂直落地，组内间距约 2 倍腿宽
//   尾：细短、垂直下垂、尾根稍粗
#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <algorithm>

namespace catside {

inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

struct Palette {
    Gdiplus::Color body     {255,  32,  32,  36};
    Gdiplus::Color outline  {255, 246, 246, 248};
    Gdiplus::Color iris     {255, 214, 232, 104};
    Gdiplus::Color pupil    {255,  20,  24,  18};
};

struct Params {
    float cx        = 184.0f;
    float groundY   = 258.0f;
    float wb        = 0.0f;     // 0=四足站立，1=小跑
    float phase     = 0.0f;
    bool  facingRight = true;
    float outlineW  = 0.0f;     // 0 = 纯剪影（参考图风格）

    float legLen    = 54.0f;    // 地面 → 躯干下缘
    float bodyRx    = 88.0f;    // 躯干半长
    float bodyRy    = 22.0f;    // 躯干半高
    float headR     = 21.0f;    // 头半径
    float headDX    = 74.0f;    // 头心相对体心的前伸距离
    float headDY    = -12.0f;   // 头心相对体心的高度（负=偏高）
    float legWF     = 15.0f;    // 前腿宽
    float legWH     = 15.0f;    // 后腿宽
    float strideF   = 18.0f;    // 前腿步幅
    float strideH   = 16.0f;    // 后腿步幅
    float liftF     = 15.0f;    // 前腿抬腿高度
    float liftH     = 13.0f;    // 后腿抬腿高度
    float eyeScale  = 1.0f;
};

inline void addEllipsePath(Gdiplus::GraphicsPath& pth, float cx, float cy, float rx, float ry) {
    pth.AddEllipse(Gdiplus::RectF(cx - rx, cy - ry, rx * 2.0f, ry * 2.0f));
}

// 胶囊（圆头粗线段）并入同一路径，与身体并集后天然无缝
inline void addCapsule(Gdiplus::GraphicsPath& pth, float ax, float ay, float bx, float by, float w) {
    float dx = bx - ax, dy = by - ay;
    float len = std::sqrt(dx * dx + dy * dy);
    float r = w * 0.5f;
    if (len < 0.001f) { addEllipsePath(pth, ax, ay, r, r); return; }
    float nx = -dy / len * r, ny = dx / len * r;
    Gdiplus::PointF pts[4] = {
        Gdiplus::PointF(ax + nx, ay + ny),
        Gdiplus::PointF(bx + nx, by + ny),
        Gdiplus::PointF(bx - nx, by - ny),
        Gdiplus::PointF(ax - nx, ay - ny)
    };
    pth.AddPolygon(pts, 4);
    addEllipsePath(pth, ax, ay, r, r);
    addEllipsePath(pth, bx, by, r, r);
}

inline void draw(Gdiplus::Graphics& g, const Params& p, const Palette& pal) {
    using namespace Gdiplus;

    const float dir = p.facingRight ? 1.0f : -1.0f;
    const float wb  = clampf(p.wb, 0.0f, 1.0f);
    const float G   = p.groundY;
    const float PI  = 3.14159265f;

    const float bodyCx = p.cx;
    const float bodyCy = G - p.legLen - p.bodyRy;
    // 小跑时躯干轻微上下起伏（小跑为斜对角弹跳，幅度小、频率为步频两倍）
    const float bob    = std::sin(p.phase * 2.0f) * 2.0f * wb;
    const float bodyY  = bodyCy + bob;

    const float hr     = p.headR;
    // 头部跟随：小跑时头随躯干弹跳再叠加一次滞后起伏，并前后轻微点动，避免"头冻住"
    const float headBob = std::sin(p.phase * 2.0f - 0.55f) * 2.4f * wb;
    const float headCy  = bodyY + p.headDY + headBob;
    const float headCx  = bodyCx + dir * p.headDX + dir * std::sin(p.phase * 2.0f + 0.35f) * 3.0f * wb;

    // ---- 步态：对角小跑（前近腿 + 后远腿 同相，前远腿 + 后近腿 反相）----
    auto legPhase = [&](float ph, float stride, float lift, float& dx, float& dy) {
        dx = stride * std::cos(ph) * wb;
        float s = std::sin(ph);
        dy = (s > 0.0f ? -lift * s : 0.0f) * wb;   // 仅支撑期反向抬腿
    };
    float dxNF, dyNF, dxFF, dyFF, dxNH, dyNH, dxFH, dyFH;
    legPhase(p.phase,      p.strideF, p.liftF, dxNF, dyNF);   // 前·近
    legPhase(p.phase + PI, p.strideF, p.liftF, dxFF, dyFF);   // 前·远
    legPhase(p.phase + PI, p.strideH, p.liftH, dxNH, dyNH);   // 后·近
    legPhase(p.phase,      p.strideH, p.liftH, dxFH, dyFH);   // 后·远

    // 四肢落点横向分布（参照参考图：组内间距约 2 倍腿宽）
    const float shoulderX = bodyCx + dir * 41.0f;
    const float hipX      = bodyCx - dir * 52.0f;
    const float innerDX   = 13.0f;                  // 同组远近腿横向错位
    const float rootY     = bodyY + p.bodyRy;       // 躯干下缘

    // ---- 组装整体剪影 ----
    GraphicsPath sil;
    sil.SetFillMode(FillModeWinding);

    // 远侧腿（先入路径，色块并集后与近侧腿同色，视觉上靠位置错位区分层次）
    addCapsule(sil, hipX - dir * innerDX, rootY, hipX - dir * innerDX + dxFH, G + dyFH, p.legWH - 2.0f);
    addCapsule(sil, shoulderX - dir * innerDX, rootY, shoulderX - dir * innerDX + dxFF, G + dyFF, p.legWF - 2.0f);

    // 尾巴：细短、垂直下垂、尾根稍粗（参考图尾长约为腿长，宽约 0.9 倍腿宽）
    {
        const float sway = std::sin(p.phase) * 2.0f * wb;
        const float tx0  = bodyCx - dir * (p.bodyRx * 0.90f);
        const float ty0  = bodyY - p.bodyRy * 0.10f;
        const float tx1  = bodyCx - dir * (p.bodyRx + 12.0f) + sway;
        const float ty1  = bodyY + p.bodyRy * 0.70f;
        const float tx2  = bodyCx - dir * (p.bodyRx + 6.0f) + sway * 1.8f;
        const float ty2  = G - 3.0f;
        const int   N    = 40;
        for (int i = 0; i <= N; ++i) {
            float t = (float)i / (float)N;
            float u = 1.0f - t;
            float x = u * u * tx0 + 2 * u * t * tx1 + t * t * tx2;
            float y = u * u * ty0 + 2 * u * t * ty1 + t * t * ty2;
            float r = (p.legWH * 0.58f) * (1.0f - 0.62f * t);
            if (r < 1.4f) r = 1.4f;
            addEllipsePath(sil, x, y, r, r);
        }
    }

    // 躯干：扁平长身的胶囊（平背 + 圆臀），比纯椭圆更贴合参考图轮廓
    addCapsule(sil, bodyCx - dir * p.bodyRx * 0.58f, bodyY,
                    bodyCx + dir * p.bodyRx * 0.58f, bodyY, p.bodyRy * 2.0f);
    addEllipsePath(sil, bodyCx - dir * p.bodyRx * 0.60f, bodyY, p.bodyRx * 0.40f, p.bodyRy);
    addEllipsePath(sil, bodyCx + dir * p.bodyRx * 0.60f, bodyY, p.bodyRx * 0.40f, p.bodyRy);

    // 近侧腿
    addCapsule(sil, hipX + dir * innerDX, rootY, hipX + dir * innerDX + dxNH, G + dyNH, p.legWH);
    addCapsule(sil, shoulderX + dir * innerDX, rootY, shoulderX + dir * innerDX + dxNF, G + dyNF, p.legWF);

    // 头：小圆头直接压在躯干前端，背线齐平，无颈
    addEllipsePath(sil, headCx, headCy, hr, hr);

    // 耳朵：尖立小三角，两耳间距约 1.3 倍头半径，略外扩
    {
        const float earH = hr * 1.05f;
        const float earW = hr * 0.62f;
        const float offs[2] = { -hr * 0.60f, hr * 0.60f };
        for (int i = 0; i < 2; ++i) {
            float ox = offs[i];
            PointF a(headCx + ox - earW * 0.5f * dir, headCy - hr * 0.44f);
            PointF b(headCx + ox + earW * 0.5f * dir, headCy - hr * 0.44f);
            PointF c(headCx + ox + dir * earW * 0.12f, headCy - hr * 0.44f - earH);
            PointF pts[3] = { a, b, c };
            sil.AddPolygon(pts, 3);
        }
    }

    // 填充剪影
    SolidBrush bodyBrush(pal.body);
    if (p.outlineW > 0.01f) {
        Pen outlinePen(pal.outline, p.outlineW * 2.0f);
        outlinePen.SetLineJoin(LineJoinRound);
        g.DrawPath(&outlinePen, &sil);
    }
    g.FillPath(&bodyBrush, &sil);

    // 眼睛：黄绿圆眼 + 深色瞳孔，位于头部中上部
    {
        const float eyeY  = headCy - hr * 0.10f;
        const float eyeDX = hr * 0.62f;
        const float er    = hr * 0.27f * p.eyeScale;
        for (int s = 0; s < 2; ++s) {
            float ex = headCx + (s == 0 ? -eyeDX : eyeDX) * dir;
            SolidBrush ring(pal.pupil);
            g.FillEllipse(&ring, RectF(ex - er, eyeY - er, er * 2.0f, er * 2.0f));
            SolidBrush ib(pal.iris);
            g.FillEllipse(&ib, RectF(ex - er * 0.92f, eyeY - er * 0.92f, er * 1.84f, er * 1.84f));
            SolidBrush pb(pal.pupil);
            float pr = er * 0.52f;
            g.FillEllipse(&pb, RectF(ex - pr, eyeY - pr * 1.15f, pr * 2.0f, pr * 2.30f));
            SolidBrush hb(Gdiplus::Color(235, 255, 255, 255));
            float hx = ex - er * 0.34f, hy = eyeY - er * 0.44f, hrad = er * 0.30f;
            g.FillEllipse(&hb, RectF(hx - hrad, hy - hrad, hrad * 2.0f, hrad * 2.0f));
        }
    }
}

} // namespace catside
