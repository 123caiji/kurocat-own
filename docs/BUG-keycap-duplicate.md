# 问题排查：键帽气泡「重复」

> 报告：打字时键帽气泡看起来有重复。本文记录排查过程与结论。

## 1. 代码层检查（已排除）

| 检查项 | 结果 |
|---|---|
| `drawFloatingKeycaps` 调用点 | 只有 1 处（`CatRenderer::draw` 内） |
| `spawnFloatingKeycap` 调用点 | 4 处：键盘 / DSH 状态 / 滚轮 / 鼠标，互斥不重叠 |
| 输入去重 | `recomputeUnifiedKeys` 只在「未按下 → 按下」跳变时触发 `onKeyDown` |
| 渲染清屏 | 每帧 `g.Clear(transparent)`，不存在残影 |

## 2. 实测（插桩）

加 `KC_BADGE_LOG=1` 后每次 spawn 记录 label 与时间：

```text
0.009 kind=0 label=Shift+A
0.563 kind=0 label=B
1.129 kind=0 label=回车
1.690 kind=0 label=空格
```

结论：**一次按键只 spawn 一次**。

再做「只触发一次按键」的隔离测试：

| 项 | 结果 |
|---|---|
| spawn 次数 | 1 次（label=A） |
| 截图目视（图像理解模型） | 屏上**只有 1 个键帽**，无重影 |

## 3. 剩下的两种可能

### A. 同时跑了两个实例

桌面上如果有两只猫（我启动的 + 你自己启动的），两个窗口会各自显示键帽，视觉上就像重复。
验证：任务管理器看 `tama.exe` 是否只有一个。

### B. 真实输入下双通道时序交错

钩子（WH_KEYBOARD_LL）与异步轮询（GetAsyncKeyState）在极快敲击时可能出现：轮询先置位、钩子随后再置位，造成同一物理键两次 down 跳变。我的环境收不到真实键盘，无法复现。

验证方法（你 1 分钟能做完）：

```bat
taskkill /IM tama.exe /F
cd /d D:\kaifa\work\kurocat-own
set KC_BADGE_LOG=1
dist\tama.exe
:: 然后敲几下 A，看 dist\badge-spawn.log
:: 若出现同一时刻两条 label=A → 确认双通道重复
```

## 4. 若确认，修法（已备好）

```cpp
// InputEngine::recomputeUnifiedKeys 加 30ms 去抖
if (isNowActive && !wasActive) {
    double now = nowSeconds();
    int idx = (int)key & 0xFF;
    if (now - lastFire_[idx] < 0.03) return;
    lastFire_[idx] = now;
    ...
}
```
