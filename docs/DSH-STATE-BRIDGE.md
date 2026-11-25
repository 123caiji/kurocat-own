# DSH 状态联动协议（Tama）

> Tama 不需要装任何插件、不联网：它只**读一个本地 JSON 文件**。写这个文件的一方可以是 DSH 插件、脚本、CI、甚至手工编辑。

## 1. 文件位置

按优先级：

1. `%APPDATA%\Tama\state.json`
2. **exe 同目录** `state.json`（便携用法，适合放在任意文件夹里跑）

两个都不存在 → 普通模式（只做键盘/鼠标反馈），不会报错。

## 2. 字段

```json
{ "state": "thinking" }
```

| state | 猫的表现 |
|---|---|
| `thinking` | 微微歪头 + 瞳孔略收 + 气泡「思考中…」 |
| `working` | 身体抖一下 + 瞳孔放大 + 气泡「干活中…」 |
| `done` | 耳朵快速抖动 + 瞳孔放大 + 气泡「完成!」 |
| `error` | 耳朵下垂 + 气泡「出错了」 |
| `idle` / 其他值 / 文件缺失 | 回到普通模式（气泡不弹） |

> 只认 `"state"` 这一个键；其余字段随便写（`task`、`progress`、`agent` 等当前忽略，方便以后扩展）。文件不合法 → 按 `idle` 处理，**永不崩溃**。

## 3. 轮询与延迟

| 项 | 值 |
|---|---|
| 轮询间隔 | 150 ms |
| 状态切换生效 | ≤ 150 ms（状态变化时才触发动画，不重复刷） |
| 失败处理 | 读不到/解析失败 → 静默回落，不写日志、不弹窗 |

## 4. 怎么写（三种方式）

### 4.1 PowerShell（最省事）

```powershell
$dir = Join-Path $env:APPDATA "Tama"
New-Item -ItemType Directory -Force -Path $dir | Out-Null
Set-Content -Path (Join-Path $dir "state.json") -Value '{"state":"working"}' -Encoding utf8
```

### 4.2 批处理 / 任意语言

```bat
echo {"state":"thinking"} > "%APPDATA%\Tama\state.json"
```

### 4.3 将来：DSH 插件壳

P2 计划里有个薄插件 `dsh-tama`：订阅 DSH 会话事件（`思考 / 工具调用 / 轮次结束 / 出错`）→ 写这个文件 → Tama 自动反应。它只做「写文件」这一件事，所以：

- 没有 electron / live2d 依赖
- 失败不影响 DSH 主流程
- 不采集任何输入内容

## 5. 隐私边界

状态文件里**只有状态枚举**，不要写窗口标题、代码内容、提示词。Tama 只读 `state` 字段，其余一概不处理。
