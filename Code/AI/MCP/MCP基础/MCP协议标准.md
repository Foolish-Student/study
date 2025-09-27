# MCP (Model Context Protocol) 协议标准

## 📖 概述

MCP (Model Context Protocol) 是一个用于AI模型与外部工具、资源进行交互的标准协议。它基于JSON-RPC 2.0规范，允许AI客户端（如Cursor）与服务器进行结构化通信。

## 🏗️ 协议架构

### 基础传输层
MCP支持多种传输方式：
- **HTTP** - 标准HTTP请求/响应（推荐用于远程服务）
- **WebSocket** - 双向实时通信
- **SSE (Server-Sent Events)** - 服务器推送事件
- **stdio** - 标准输入输出（用于本地进程）

### 消息格式
所有MCP消息都遵循JSON-RPC 2.0格式：

```json
{
  "jsonrpc": "2.0",           // 协议版本，固定值
  "id": 1,                    // 请求ID，用于匹配请求和响应
  "method": "method_name",    // 方法名称
  "params": {...},            // 参数（可选）
  "result": {...},            // 响应结果（仅响应消息）
  "error": {...}              // 错误信息（仅错误响应）
}
```

## 🔄 协议流程

### 1. 连接初始化
```
Client → Server: initialize
Server → Client: 返回协议版本和服务器能力
```

### 2. 能力发现
```
Client → Server: tools/list, resources/list, prompts/list
Server → Client: 返回可用的工具/资源/提示列表
```

### 3. 功能调用
```
Client → Server: tools/call, resources/read, prompts/get
Server → Client: 返回执行结果
```

## 📋 核心消息类型

### 1. 初始化消息 (`initialize`)

**请求**:
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "protocolVersion": "2024-11-05",
    "capabilities": {},
    "clientInfo": {
      "name": "cursor",
      "version": "1.0"
    }
  }
}
```

**响应**:
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "protocolVersion": "2024-11-05",
    "capabilities": {
      "tools": {},
      "resources": {},
      "prompts": {}
    },
    "serverInfo": {
      "name": "my-mcp-server",
      "version": "1.0.0"
    }
  }
}
```

### 2. 工具列表 (`tools/list`)

**请求**:
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "tools/list"
}
```

**响应**:
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "tools": [
      {
        "name": "calculator",
        "description": "执行数学计算",
        "inputSchema": {
          "type": "object",
          "properties": {
            "operation": {"type": "string", "enum": ["add", "subtract"]},
            "a": {"type": "number"},
            "b": {"type": "number"}
          },
          "required": ["operation", "a", "b"]
        }
      }
    ]
  }
}
```

### 3. 工具调用 (`tools/call`)

**请求**:
```json
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "tools/call",
  "params": {
    "name": "calculator",
    "arguments": {
      "operation": "add",
      "a": 10,
      "b": 5
    }
  }
}
```

**响应**:
```json
{
  "jsonrpc": "2.0",
  "id": 3,
  "result": {
    "content": [
      {
        "type": "text",
        "text": "10 + 5 = 15"
      }
    ],
    "isError": false
  }
}
```

## 🛠️ 工具定义规范

### 工具结构
```json
{
  "name": "tool_name",              // 工具唯一标识符
  "description": "工具描述",         // 工具功能说明
  "inputSchema": {                  // JSON Schema定义输入参数
    "type": "object",
    "properties": {
      "param1": {
        "type": "string",
        "description": "参数1描述"
      }
    },
    "required": ["param1"]
  }
}
```

### 输入Schema类型
- `string` - 字符串
- `number` - 数字
- `boolean` - 布尔值
- `object` - 对象
- `array` - 数组
- `enum` - 枚举值

## 🔍 资源系统

### 资源列表 (`resources/list`)
```json
{
  "jsonrpc": "2.0",
  "method": "resources/list",
  "result": {
    "resources": [
      {
        "uri": "file://path/to/file.txt",
        "name": "配置文件",
        "description": "系统配置文件",
        "mimeType": "text/plain"
      }
    ]
  }
}
```

### 资源读取 (`resources/read`)
```json
{
  "jsonrpc": "2.0",
  "method": "resources/read",
  "params": {
    "uri": "file://path/to/file.txt"
  },
  "result": {
    "contents": [
      {
        "uri": "file://path/to/file.txt",
        "mimeType": "text/plain",
        "text": "文件内容..."
      }
    ]
  }
}
```

## 💬 提示系统

### 提示列表 (`prompts/list`)
```json
{
  "jsonrpc": "2.0",
  "method": "prompts/list",
  "result": {
    "prompts": [
      {
        "name": "code_review",
        "description": "代码审查提示",
        "arguments": [
          {
            "name": "code",
            "description": "要审查的代码",
            "required": true
          }
        ]
      }
    ]
  }
}
```

## ⚠️ 错误处理

### 错误响应格式
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "error": {
    "code": -32601,
    "message": "Method not found",
    "data": "详细错误信息"
  }
}
```

### 标准错误码
- `-32700` - Parse error (解析错误)
- `-32600` - Invalid Request (无效请求)
- `-32601` - Method not found (方法未找到)
- `-32602` - Invalid params (无效参数)
- `-32603` - Internal error (内部错误)

## 🔧 协议版本

当前标准版本：`2024-11-05`

### 版本兼容性
- 服务器必须声明支持的协议版本
- 客户端应检查版本兼容性
- 不兼容时应优雅降级或报错

## 📡 传输层细节

### HTTP传输
- 使用POST方法
- Content-Type: `application/json`
- 支持CORS跨域
- 每个请求-响应为独立事务

### WebSocket传输
- 支持双向通信
- 可以发送通知消息
- 保持长连接

### SSE传输
- 服务器主动推送
- 适合实时更新场景
- 单向通信

## 🎯 最佳实践

1. **错误处理**：始终提供有意义的错误消息
2. **参数验证**：严格验证输入参数
3. **性能优化**：避免阻塞操作
4. **安全考虑**：验证权限和输入安全性
5. **日志记录**：记录关键操作和错误
6. **文档完善**：提供清晰的工具描述

## 📚 参考资源

- [MCP官方规范](https://spec.modelcontextprotocol.io/)
- [JSON-RPC 2.0规范](https://www.jsonrpc.org/specification)
- [JSON Schema规范](https://json-schema.org/)
