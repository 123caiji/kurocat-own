# CHANGELOG

> 回滚锚点：每个版本都留了 git tag，可直接 `git reset --hard <tag>`。

## [F1-键帽气泡] · 2026-11-25

**新增 / 重做**

- 键帽气泡升级为自有设计：更大圆角（7）、1.2px 描边、双层软投影、顶部高光、出生 0.1s 的「按下位移」弹起
- **组合键合并**：按住 Ctrl/Shift/Alt/Win 时，普通键弹出一颗宽键帽（如 `Ctrl+Shift+A`），修饰键本身不再刷屏
- **鼠标三键分色**：左键=蓝、右键=红、中键=琥珀；滚轮=青绿并显示方向（读既有 Raw Input 字段）
- 新增两款原创皮肤：`robin.kurocat`（知更夜蓝猫）、`stage.kurocat`（舞台夜色猫）

**文档 / 素材**

- `docs/media/demo-keycaps.gif`（键帽气泡录制）
- `docs/REBRAND+LICENSE.md`（品牌自有化清单 + 合规；含 gitee virtual-maiden 核实结论）

**编译**：0 error；单文件 1,293 KB

## [P0-v1-removed] · 2026-11-25

**移除（设计否决）**

- 顶部「操作信息条」整体撤下（反馈：太丑）。源码回滚到基线，删 `ActionHud.hpp`、`onWheel`、测试宏与对应演示 GIF。
- 保留其中唯一有价值的改动：**禁用上游自动更新通道**（安全）。

**下一步**

- 视觉主线改走「键帽气泡」，规划见 [PLAN-20261125.md](PLAN-20261125.md)。

## [P0-v1] · 2026-11-25

**新增**

- 🏷️ 操作信息条（`windows/src/ActionHud.hpp`）：键名主字 + 动作/组合副字 + 鼠标 L/R/M 状态灯 + 滚轮方向；常驻不消失（空闲显示「等待输入」，超时降到 55% 透明度）
- `InputEngine::onWheel` 回调（读既有 Raw Input 字段，不新增采集）

**安全**

- 禁用上游自动更新通道（原实现从作者私有域名下载 exe，仅校验长度与 MZ 魔数后自动替换执行）
- 自有分支现为完全离线：无任何出站网络请求

**性能**

- 字体 / StringFormat / 文本宽度改为静态缓存（首版每帧新建 GDI+ Font 导致单核 100%，已修）
- 同机对比：基线 50.0% → 新版 41.1%（单核基准，差异在噪声内）；内存 49.3 MB → 50.6 MB

**文档 / 素材**

- `docs/P0-V1-NOTES.md`（改动、实测、安全、回滚）
- `docs/media/demo-idle.gif`（转头眨眼）、`docs/media/demo-info.gif`（信息条轮播）
- `tools/capture.cpp`、`tools/img.mjs`、`tools/perf.cpp`（抓帧 / 零依赖 GIF 编码 / 进程性能采样）

**回滚**

```bat
git reset --hard pre-p0-20261125
```

## [v1.4.0-own1] · 2026-11-23

- 从 chanke17/KuroCat v1.4.0（MIT）移植 Windows 原生源码
- 品牌与命名空间自有化（窗口标题 / 注册表 / 单实例锁 / 数据目录）
- 皮肤库支持 exe 同目录 `skins\`（随包分发）
- 新增 3 款皮肤：暖奶茶布偶猫 / 三花猫 / 奶牛猫
