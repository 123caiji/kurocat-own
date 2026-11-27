# 漫步（溜达）为什么不动 —— 根因与修复（v0.12.0）

## 现象

- 开了「闲时自己溜达」，猫一动不动
- 手动「让它逛逛」也没反应
- 日志里 `app.wandering=1`、目标坐标也正确，但窗口矩形始终不变

## 根因（关键）

渲染每帧最后调用：

```cpp
POINT ptDst = { wr.left, wr.top };      // wr 是本帧开头 GetWindowRect 抓的
UpdateLayeredWindow(hwnd, screenDC, &ptDst, ...);
```

**UpdateLayeredWindow 会按 ptDst 重新把窗口移到那个位置。**
漫步/下落是在 tick 中途移动窗口的，于是刚移过去就被这一帧的 ULW 用旧坐标写回去了——每帧「走 5px 又被拽回 5px」，看起来就是完全不动。

这也解释了为什么**拖拽**没问题：拖拽时窗口位置是在 tick 之前（鼠标消息里）改的，ptDst 抓到的是新位置。

## 修复

```cpp
// 呈现前重新读一次窗口位置，mid-frame 的移动才不会被覆盖
GetWindowRect(app.hwnd, &wr);
POINT ptDst = { wr.left, wr.top };
```

## 实测

修复前（每 500ms 采样）：

```text
1526,35  1526,35  1526,35  1526,35  ...（不动）
```

修复后：

```text
1526,35 → 1501,50 → 1345,52 → 1260,49 → 1105,51 → 985,39
```

**窗口确实在走，且是逐步移动（每帧 5px），不是瞬移。**

## 附带收益

同一个 bug 也解释了：

1. 以前「让它逛逛」偶尔只动 1-2px
2. 重力下落时窗口位置偶尔「跳回原地」
3. 某些情况下猫看起来像「闪了一下又回来」

## 顺带说明：crash.log 现在带偏移

```text
[2026-11-27 17:30:12] code=0xC0000005 base=0x7FF6A1230000 off=0x2A12D activity=renderer.draw
```

拿到 `base` 与 `off` 就能用 `addr2line` 直接定位到函数行。
