# KuroCat Own

一只自用的 Windows 桌面猫：打字/鼠标操作实时反馈，数据驱动的皮肤系统。

**底座**：chanke17/KuroCat v1.4.0 的 Windows 原生实现（C++17 + Win32 + GDI+ + Raw Input），已按自有品牌改造。
**皮肤**：`.kurocat` 数据文件（JSON），随包放 `dist/skins/`，也可从「换装间」导入。

## 构建

需要 MinGW g++（或 MSVC），双击：

```bat
build-own.cmd
```

产物：`dist/kurocat-own.exe` + `dist/skins/*.kurocat`

## 运行

双击 `run-own.cmd`（会先结束旧实例）。

## 皮肤

内置 4 款（经典小黑猫 / 美短 / 橘白 / 挖煤暹罗），另附 3 款：

| 文件 | 名称 | 说明 |
|---|---|---|
| `skins/ragdoll.kurocat` | 暖奶茶布偶猫 | 奶油底 + 深色重点色 + 蓝眼 |
| `skins/calico.kurocat` | 三花猫 | 白底 + 橘/黑斑块分区 |
| `skins/cow.kurocat` | 奶牛猫 | 黑白燕尾服 + 白手套 + 门脸白条 |

换肤：右键小猫 →「猫咪皮肤」；或把 `.kurocat` 拖进「换装间」。

## 与官方版的隔离

| 项 | 取值 |
|---|---|
| 窗口标题 | `KuroCat Own` |
| 注册表 | `HKCU\Software\KuroCatOwn` |
| 单实例锁 | `Local\KuroCatOwn_SingleInstance` |
| 数据目录 | `%APPDATA%\KuroCatOwn\` |

可与官方 KuroCat 同时安装、互不干扰。

## 许可

MIT。来源与改动见 `NOTICE.md`，原始版权见 `LICENSE`。
