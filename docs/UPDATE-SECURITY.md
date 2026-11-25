# 更新检查 · 安全设计（Tama）

> 一句话：**只查询版本号 + 打开固定下载页；永不下载、永不执行任何远端文件。**

## 1. 数据流

```text
Tama --(1) HTTPS GET--> github.com/123caiji/tama/releases/latest
                        ↳ 302 跳转 → 取最终 URL 的最后一段作为最新 tag
Tama --(2) HTTPS GET--> github.com/chanke17/KuroCat/releases/latest
                        ↳ 同上，仅用于「上游有新版」提示
结果 → 本地比较版本号 → 弹窗询问是否打开固定下载页 → ShellExecuteW(固定 URL)
```

## 2. 六条安全约束（代码里逐条对应）

| # | 约束 | 实现 |
|---|---|---|
| S1 | **只允许 HTTPS + 固定 host** | 只用 `github.com:443`，`WINHTTP_FLAG_SECURE`；不接受任何用户/响应提供的域名 |
| S2 | **不信任响应里的 URL** | 打开下载页时使用**编译期常量** `OWN_RELEASES_URL`，响应里的 `html_url` 一律不用 |
| S3 | **不下载、不替换、不执行** | 更新 = `ShellExecuteW(open, 固定URL)`；没有任何 `CreateFile`/`MoveFile`/`CreateProcess` 参与更新 |
| S4 | **响应体丢弃 + 有界读取** | 只取最终跳转 URL 的 tag 段；带读取上限 64KB；tag 只保留 `[0-9A-Za-z.-_+]`，长度 ≤32 |
| S5 | **不消耗 API 配额、不需要 token** | 用 `releases/latest` 的 302 跳转取 tag，而不是 `api.github.com`（避免 403 速率限制，也没有任何凭据可泄露） |
| S6 | **节流 + 失败静默** | 自动检查 6 小时一次（`HKCU\Software\Tama\LastUpdateCheck`）；任何失败都不提示、不影响使用；手动检查才回显错误 |

## 3. 上游同步提醒

- 每次检查同时看 `chanke17/KuroCat` 的最新 tag；
- 与**移植基线** `1.4.0` 比较：更新则提示「上游有 vX，本项目基线 1.4.0，仅供知悉，不会自动安装」；
- 这条信息只出现在手动检查与「发现自有新版」的弹窗里，**不改变本软件任何行为**。

## 4. 版本比较规则

按数字段比较（忽略 `v` 前缀与连字符后缀）：`v0.4.0` > `v0.2.1`；`v1.4.0-own1` 视为 `1.4.0`。
因此**旧 tag 不会把新版本「判成旧版」**，但请保持 tag 语义一致（本项目用 `v0.x` 系列）。

## 5. 用户可见行为

| 情形 | 表现 |
|---|---|
| 有自有新版 | 弹窗：版本号 + 是否打开 GitHub 下载页（默认焦点在「否」） |
| 已是最新（手动检查） | 弹窗：当前版本；若上游有新版会附一行提示 |
| 网络失败 | 手动检查时提示「无法连接，已静默失败」；自动检查完全静默 |
| 自动检查间隔 | 6 小时内不重复（注册表节流） |

## 6. 诊断命令

```bat
:: 不发任何弹窗，把结果写到 exe 同目录 update-check.log
dist\tama.exe --check-update
```

日志字段：`appVersion` / `ownRepo ok/latest/hasUpdate` / `upstream ok/latest/newerThanBaseline` / `httpStatus`。

## 7. 已知边界（诚实说明）

1. 本机制**不验证安装包签名**——因为它根本不下载安装包。真要做「应用内更新」，必须先有代码签名或 minisign/Ed25519 这类**独立于下载通道**的信任根，否则等于没验。
2. GitHub 页面本身若被劫持，用户看到的下载页会是假的；这属于浏览器侧威胁，应用侧只能靠「固定 URL + HTTPS」降低风险。
3. 不提供静默/自动安装，是为了让每一次执行都经过用户明确点击。
