# 上游 KuroCat 更新差异（v1.4.0 → v1.4.3）与移植建议

> 检索方式：在本地克隆里 `git fetch --tags`（只读，不改原仓库），对比 `v1.4.0..origin/main`。
> 结论：上游 3 个版本共 **10 次提交 / 28 个文件 / +14,892 −1,156 行**，其中 Windows 侧新增了**原生设置界面**与**`.kurokb` 键盘皮肤系统**，并重写了渲染器。

## 1. 版本时间线（2026-11-24/25）

| 版本 | 提交 | 主要变化 |
|---|---|---|
| v1.4.1 | `412bc38` | 修尾巴卷曲接缝、双色毛保留白色前腿 |
| v1.4.2 | `92623c4` | Windows 原生自更新（替换 .bat）、VERSIONINFO 资源、manifest 加 asInvoker、皮肤工坊提到一级菜单 |
| v1.4.3 | `8b8dded` | 更贴耳的「后颈拖拽」、橘白背脊毛色、浅色毛睡觉眼皮可见；合并两个外部 PR（macOS 睡眠帧率、白手臂/尾根） |

> 提交里还有一条 `Release v1.5.0: Douni style, .kurokb keyboard skins, 2D drag physics, rotate edit box, native Settings UI`（`ae09010`），但**没有对应的 v1.5.0 tag**，最终以 v1.4.1–1.4.3 发布。

## 2. 新增能力（Windows 侧，带位置）

| 能力 | 证据 | 规模 |
|---|---|---|
| **原生设置窗口** | `windows/src/main.cpp` 中 `SETTINGS_WINDOW_CLASS`、`showSettingsWindow()`（540×618，Win32 控件：6 个 COMBOBOX + 33 个 BUTTON + 分组框，跟随系统深色模式与 DWM 标题栏） | ~170 行 UI + 刷新逻辑 |
| **`.kurokb` 键盘皮肤** | 新增 `shared/KeyboardSkinSpec.hpp`（176 行）+ `windows/src/SkinLibrary.hpp`（+114）+ `KeyMap.hpp`（+57）+ 渲染器按 skin 绘制键盘 | 全链路，渲染器耦合 |
| **2D 拖拽物理** | `CatPhysics.hpp` 新增 `scruffSwingY`、`scruffTailSwing`、`scruffDragDy_`、`scruffDragAccumX/Y_` 等弹簧与累计量 | ~80 行物理 + 渲染钩子 |
| **编辑框旋转** | `settings.rotationDeg`（−180..180，注册表 `RotationTenths`） | 小 |
| **VERSIONINFO 资源** | 新增 `windows/resource.rc`（文件属性：FileVersion/ProductName/LegalCopyright） | 34 行 |
| **manifest 强化** | 加 `<trustInfo><requestedExecutionLevel level="asInvoker" uiAccess="false"/>`，assemblyIdentity 改为 `com.chanke.kurocat` | 9 行 |
| 新增毛色 / 修复 | Douni（小逗泥）、橘白背脊、浅色毛睡觉眼皮、白前腿、尾巴接缝、Siamese 生气修复 | 都在渲染器大改里 |
| 原生自更新 | 替换 `.bat` 更新器（配合 SignPath 代码签名） | 我们**不采用**（安全策略不同） |

## 3. 与 Tama 现状的冲突面

| 上游改动 | 我们的对应 | 冲突风险 |
|---|---|---|
| 渲染器整体重写（+3,507 行） | 我们改过 `drawFloatingKeycaps`（键帽气泡 F1） | **高**：整段合入会覆盖我们的键帽设计 |
| 设置界面 | 我们把菜单重构成 5 个分组 | 低：属于新增窗口，不动菜单也能并存 |
| `.kurokb` | 我们没有键盘皮肤系统 | 中：需要同时改 KeyMap/渲染器键盘绘制 |
| VERSIONINFO / manifest | 我们只有 manifest（v1.4.0 版，无 trustInfo） | **低**：纯增量，风险最小 |
| 自更新 | 我们已改为「只查不装」 | 明确不采用 |

## 4. 移植建议（按性价比排序）

| 优先级 | 项 | 理由 | 预估 | 风险 |
|---|---|---|---|---|
| **P0-A** | **VERSIONINFO 资源 + manifest trustInfo** | 纯增量；exe 属性页有版本/版权；显式 asInvoker 更规范 | 0.5h | 极低 |
| **P0-B** | **原生设置窗口（裁剪版）** | 直接解决「一长溜菜单」的观感；Win32 原生控件 + 深色模式 + 居中 | 1–2 天 | 低（独立窗口，不动渲染） |
| P1 | 2D 拖拽物理（后颈甩动） | 「抓起来甩」的手感提升明显；只需 CatPhysics + 少量渲染钩子 | 1 天 | 中 |
| P2 | `.kurokb` 键盘皮肤 | 生态价值最高，但需要键盘绘制全面改造；建议**等我们先把键盘绘制重构**再做 | 3–5 天 | 高 |
| 跳过 | 渲染器整体重写 / Douni 毛色 / 自更新 | 会覆盖我们自有视觉与安全策略；Douni 可后续单独照着重绘一个自己的毛色 | — | — |

## 5. 可复用的参考命令（不改原仓库，只看）

```bat
:: 看设置窗口实现
git -C D:\kaifa\work\kurocat-merge\repos\KuroCat-chanke show origin/main:windows/src/main.cpp | findstr /N SETTINGS_WINDOW_CLASS

:: 看 .kurokb 数据结构
git -C ... show origin/main:shared/KeyboardSkinSpec.hpp

:: 看 VERSIONINFO 资源
git -C ... show origin/main:windows/resource.rc
```

## 6. 本次已顺手落地的两项（P0-A）

1. `windows/tama.rc`：为 Tama 增加 VERSIONINFO（产品名 Tama、版本 0.4.0、版权含上游 MIT 声明）
2. `windows/app.manifest`：补 `<trustInfo>` 显式 `asInvoker`（不请求提权），assemblyIdentity 改为 Tama

这两项不触碰任何渲染/交互代码，可随时回滚。
