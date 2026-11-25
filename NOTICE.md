# NOTICE · 来源与署名

本工程是 **chanke17/KuroCat** 的自有分支，基于其 v1.4.0 的 Windows 原生源码移植：

- 上游：https://github.com/chanke17/KuroCat （MIT，Copyright (c) 2026 chanke17）
- 移植内容：`windows/src`、`shared`、`skins`、`LICENSE`、`docs/skin-format.md`
- 本项目改动：
  1. 品牌与命名空间自有化（窗口标题 `KuroCat Own`、注册表 `Software\KuroCatOwn`、单实例锁、自启键值、数据目录）
  2. 皮肤库支持「exe 同目录 `skins\`」随包分发（`SkinLibrary.hpp`）
  3. 新增 3 款皮肤数据：`ragdoll.kurocat`（暖奶茶布偶猫）、`calico.kurocat`（三花猫）、`cow.kurocat`（奶牛猫），配色与皮肤阵容参考
     **qianyiai/kurocat**（MIT，Copyright (c) 2026 KuroCat Contributors）的 7 款皮肤设定

MIT 许可要求保留原始版权声明：见本目录 `LICENSE`。二次分发请一并带上本文件。
