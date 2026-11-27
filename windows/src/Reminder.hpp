#pragma once
// Reminder.hpp —— 第 1 层提醒（纯本地定时，零依赖、不联网）
// 设计：只处理「什么时候提醒」，表现交给 CatPhysics + 托盘气泡。
#include <cmath>
#include <cstdint>

namespace tama_remind {

enum class Kind { Rest = 0, Water = 1, Sit = 2, Test = 3 };

struct Engine {
    double typingSessionStart = 0.0;  // 本轮连续打字起点（0 = 未开始）
    double nextWaterTime = 0.0;       // 下次喝水提醒（绝对时间）
    double lastActivityTime = 0.0;    // 最近一次有任何输入
    double dndUntil = 0.0;            // 免打扰截止
    double lastFiredTime = 0.0;       // 防止同一秒重复触发
    int firedThisSession = 0;

    bool inDnd(double now) const { return now < dndUntil; }
    void startDnd(double now, double seconds) { dndUntil = now + seconds; }
    void cancelDnd() { dndUntil = 0.0; }
    double dndRemaining(double now) const { return now < dndUntil ? dndUntil - now : 0.0; }
};

inline void resetWater(Engine& e, double now, int minutes) { e.nextWaterTime = now + (double)minutes * 60.0; }

}  // namespace tama_remind
