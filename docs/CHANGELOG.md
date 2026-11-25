# CHANGELOG

> 回滚锚点：每个版本都留了 git tag，可直接 `git reset --hard <tag>`。

## [v0.5.0 · 原生设置窗口] · 2026-11-25

**新增**
- **原生设置窗口**（`windows/src/SettingsWindow.hpp`，322 行）：Win32 原生控件，3 个分组（外观 / 行为与反馈 / 系统）+ 27 个控件，随 DPI 缩放
  - 与右键菜单共用 `applyAppCommand(app, CMD_*)`，行为完全一致
  - 菜单顶层新增「⚙️ 设置…」；启动参数 `--settings` 可直接打开
- `tools/wchildren.exe`（枚举子控件验证）、`tools/shotbyclass.cpp`（按类名截图）

**视觉**
- 新皮肤演示动图 `docs/media/demo-robin.gif`（24 帧 / 163 KB）

**文档**
- `docs/SETTINGS.md`（功能表、实现要点、实测控件清单）

**已知问题**
- 自动化环境里 PrintWindow / 屏幕 BitBlt 抓不到原生子控件，仓库暂缺设置窗口截图（窗口本身可用）

## [v0.4.0 · 安全更新检查] · 2026-11-25

**新增**
- 「检查更新」改为**自有渠道 + 上游同步提示**：
  - 自有：`123caiji/tama` 的 `releases/latest`（302 跳转取 tag，不用 API、不需要 token）
  - 上游：`chanke17/KuroCat` 最新 tag 与移植基线 `1.4.0` 比较，仅提示不安装
- 自动检查 6h 节流（注册表 `LastUpdateCheck`），失败静默
- 诊断入口 `--check-update`（写 `update-check.log`，不弹窗）

**补：移植上游两项低风险改进（P0-A）**
- `windows/tama.rc`：VERSIONINFO（产品名 Tama / 版本 0.4.0 / 版权含上游 MIT 声明）→ exe 属性页可见
- `windows/app.manifest`：补 `<trustInfo>` 显式 `asInvoker`（默认不提权），assemblyIdentity 改为 Tama

**安全（重点）**
- 只查询、只打开**编译期固定 URL**；不下载/不替换/不执行任何远端文件
- 仅 HTTPS + 固定 host；响应体丢弃、上限 64KB；tag 严格清洗（`[0-9A-Za-z.\-_+]`，≤32）
- 详见 [UPDATE-SECURITY.md](UPDATE-SECURITY.md)

## [v0.3 · 菜单与皮肤打磨] · 2026-11-25

**修复**
- 版本号此前仍显示 `v1.4.0-own1`（改名时漏改）→ 现为 `0.2.1`

**界面**
- **右键菜单重构**：原来一长溜（20+ 平铺项）改成 5 个分组 —— 皮肤 / 外观 / 窗口 / 系统 / 语言；顶层只剩状态行 + 5 个入口 + 退出
  - 外观：键帽气泡 / 键位图例 / 键盘跑马灯 / 键盘配色 / 瞳色 / 数位板 / 调整大小与方向
  - 窗口：显示隐藏 / 鼠标穿透 / 窗口置顶 / 全屏自动隐藏
  - 系统：开机自启 / 以管理员运行 / OBS 直播捕获 / 检查更新

**皮肤**
- 「知更鸟 · 夜航」重做：不再是一块死板色块，改用**分层着色**（底色竖向渐变 + 胸羽曲线分区 + 高光层 + 冠部暗块 + 尾羽浅带 + 眉纹），观感接近参考项目的打光层次；实现全部是自有 `.kurocat` 数据，未复制任何素材

**工具**
- 新增 `tools/skincheck.cpp`：用运行时同一套解析器校验全部皮肤（当前 **9 款全部 OK**）

## [v0.2 · Tama] · 2026-11-25

**品牌**
- 项目更名为 **Tama**：窗口标题、托盘、注册表 `HKCU\\Software\\Tama`、单实例锁 `Local\\Tama_SingleInstance`、数据目录 `%APPDATA%\\Tama\\`、自启键值、类名、exe 名（`tama.exe`）、脚本（`build.cmd` / `run.cmd`）
- 安全：上游更新域名替换为 `disabled.invalid`（原通道已禁用，双保险）

**新增**
- **DSH 状态联动**：每 150ms 读 `state.json`，思考/干活/完成/出错 → 猫的表情与气泡；协议见 [DSH-STATE-BRIDGE.md](DSH-STATE-BRIDGE.md)

**回滚**
```bat
git reset --hard f1-keycaps
```

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
