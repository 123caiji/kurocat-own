# CHANGELOG

> 回滚锚点：每个版本都留了 git tag，可直接 `git reset --hard <tag>`。

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
