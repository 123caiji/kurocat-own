# 品牌自有化 · 清单与开源合规（2026-11-25）

> 目标：把工程从「上游分支」变成**自己的项目**——名字、命名空间、文档口吻全部自有；上游只保留**代码与皮肤原始出处**的致谢（MIT 义务）。

## 1. 已完成的（本轮）

| 项 | 状态 |
|---|---|
| README 口吻 | ✅ 改为「致谢与来源」，明确只引用代码 + 4 款基础皮肤 |
| 新增原创皮肤 | ✅ `robin.kurocat`（知更夜蓝猫）、`stage.kurocat`（舞台夜色猫）—— 纯配色/矢量，无第三方素材 |
| 键帽气泡视觉 | ✅ 升级为自有设计（圆角/投影/按下位移/组合键合并/鼠标分色） |
| 上游自动更新 | ✅ 已禁用（安全） |

## 2. 待改名（需要产品名定稿后一次性替换）

| 位置 | 现值 | 替换为 |
|---|---|---|
| 窗口标题 | `KuroCat Own` | <新产品名> |
| 托盘提示 | `KuroCat ...` | <新产品名> |
| 注册表 | `HKCU\Software\KuroCatOwn` | `HKCU\Software\<NewKey>` |
| 单实例锁 | `Local\KuroCatOwn_SingleInstance` | `Local\<NewKey>_SingleInstance` |
| 数据目录 | `%APPDATA%\KuroCatOwn\` | `%APPDATA%\<NewKey>\` |
| 自启键值 | `KuroCatOwn` | `<NewKey>` |
| 类名 | `KuroCatTrayOwnerClass` / `KuroCatOverlayWindowClass` / `KuroCatDressingRoomClass` | `<NewKey>...` |
| 版本属性 | `KuroCatVersion` | `<NewKey>Version` |
| 仓库名 / exe 名 | `kurocat-own` / `kurocat-own.exe` | `<new-name>` / `<new-name>.exe` |
| 皮肤作者字段 | `kurocat-own` | <新产品名> |

> 替换点集中在 `windows/src/main.cpp`、`CatPhysics.hpp`、`SkinLibrary.hpp`、`build-own.cmd`、`run-own.cmd`、`README/NOTICE/CHANGELOG`。改完需要**重新编译 + 迁移一次数据目录**（旧设置不会自动搬）。

## 3. 开源合规（必须保留的）

| 义务 | 做法 |
|---|---|
| MIT 版权声明 | `LICENSE` 原样保留（Copyright (c) 2026 chanke17） |
| 来源与改动说明 | `NOTICE.md` 保留并更新（说明哪些文件来自上游、哪些是本项目新增） |
| README 致谢 | 「🙏 致谢与来源」段落，链接两个上游仓库 |
| 不可做的 | 不删除 LICENSE/NOTICE；不声称「全部代码原创」；不把上游素材当自有资产再分发 |

## 4. 关于 https://gitee.com/lv-hang12316/virtual-maiden（虚拟姬）

**核实结果**：该仓库**未声明任何开源许可证**（Gitee 页面明确提示「该仓库未声明开源许可证文件」），只有 1 次提交，结构为 Electron + `public/models/知更鸟音乐会`（疑似 Live2D 模型资源）。

| 结论 | 说明 |
|---|---|
| **不能复制其代码/模型/图片** | 无许可证 = 默认保留所有权利；且模型疑似第三方角色素材，风险更高 |
| **可以借鉴的** | 产品方向（AI 桌宠 + 聊天）、配色氛围、界面布局思路 |
| **可落地的** | 用**颜色与风格意向**做我们自己的矢量皮肤（颜色本身不受版权保护） |

已据此产出两款原创皮肤（见第 1 节），配色为「夜蓝 + 暖橙胸羽」与「深紫夜色 + 霓虹蓝粉」，形象仍是我们自己的猫，不涉及任何角色复刻。

> 建议：如果你想要「知更鸟」这种具体角色形象，需要**自己画/委托画**，或用明确允许二创的素材库（CC0 / CC-BY，并标注来源）。

## 5. 下一步

1. 你定产品名 → 我做一次性全量替换（1 个 commit，含数据目录迁移提示）。
2. 继续 F2（DSH 状态联动）与 F3（KPM 角标）。
3. 皮肤工坊（网页）留到 P1，先出只读预览版。
