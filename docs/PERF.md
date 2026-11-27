# 性能与体积优化（v0.8.0）

> 参考上游 v1.5.x 的「跨平台性能大改 + 体积瘦身」，挑了两个**低风险、纯收益**的点移植。

## 1. 自适应帧率（核心改动）

之前是**固定 60Hz 一直渲染**，空闲时也在跑满。现在按状态降帧：

| 状态 | 帧率 | 判定条件 |
|---|---|---|
| 活跃 | **60 Hz** | 有按键/鼠标按下/拖拽/编辑/演示/近 1.5s 内有输入/抚摸中/悬空 |
| 空闲（醒着） | **20 Hz** | 以上都不满足 |
| 睡眠 | **10 Hz** | `sleepiness > 0.5` |
| 隐藏 | **4 Hz** | 不可见或全屏自动隐藏 |

实现（`windows/src/main.cpp`）：

```cpp
double desiredFrameInterval(const AppState& app, double now) { ... }

void renderIfDue(AppState& app) {
    double now = nowSeconds();
    if (now < app.nextFrameTime) return;
    double interval = desiredFrameInterval(app, now);
    app.nextFrameTime = (now - app.nextFrameTime < interval) ? app.nextFrameTime + interval : now + interval;
    tickAndRender(app);
}
```

主循环的等待也从「最多 1 帧」改成「最多一个自适应间隔」，避免空转。

## 2. 体积瘦身

build.cmd 增加：

```text
-ffunction-sections -fdata-sections -Wl,--gc-sections -s
```

作用：把未被引用的函数/数据整段裁掉（含已废弃的应用内更新代码路径），并 strip 符号表。

## 3. 实测数据（本机，8 秒空闲采样，取中位数）

| 版本 | exe 大小 | 空闲 CPU（单核） | 常驻内存 |
|---|---|---|---|
| v0.7.0（固定 60Hz） | 1,377 KB | **43.8%** | 28.9 MB |
| v0.8.0（自适应 + gc + strip） | **723 KB** | **10.2%** | 28.9 MB |
| 变化 | **−47.5%** | **−76.7%** | 持平 |

> 说明：CPU 数字在本机有波动（多次采样 6.3%–16.1%），上表取中位数；对比方法是同一台机器、同一测试工具（`tools/perf.exe`，8 秒采样）。

## 4. 为什么不会影响手感

- 只要**有任何输入**（键/鼠标/拖拽/触碰），立刻回到 60Hz；
- 空闲降帧只影响「猫自己呼吸/眨眼」的频率，人眼几乎无感；
- 睡眠与隐藏状态本来就该慢。

## 5. 未采用的优化（记录一下）

| 项 | 原因 |
|---|---|
| 渲染器重构（上游 +2797 行） | 与我们的键帽/悬空姿态改动冲突，风险高 |
| LTO（`-flto`） | 实测 723 KB vs 717 KB，没有收益 |
| `-Os` | 体积 565 KB 更小，但 CPU 中位数 14.6% 比 `-O2` 的 10.2% 差，得不偿失 |
