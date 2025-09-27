# MCP服务器开发手册

## 📖 基于 main.go 的完整实现指南

本手册基于你的 `main.go` 源码，详细说明如何开发一个完整的MCP服务器。

## 🏗️ 核心架构

### 项目结构
```
project/
├── main.go              # 主服务器代码
├── go.mod               # Go模块定义
├── .cursor/mcp.json     # Cursor配置文件
└── README.md            # 项目说明
```

## 📋 必须实现的数据结构

### 1. 基础消息结构

```go
// MCP协议的基本消息结构 - 所有通信的基础
type MCPMessage struct {
    JSONRPC string      `json:"jsonrpc"`           // 固定值 "2.0"
    ID      interface{} `json:"id,omitempty"`      // 请求ID，用于匹配请求响应
    Method  string      `json:"method,omitempty"`  // 方法名，如 "initialize"
    Params  interface{} `json:"params,omitempty"`  // 请求参数
    Result  interface{} `json:"result,omitempty"`  // 响应结果
    Error   *MCPError   `json:"error,omitempty"`   // 错误信息
}

// 错误结构 - 标准化错误响应
type MCPError struct {
    Code    int         `json:"code"`              // 错误代码
    Message string      `json:"message"`           // 错误描述
    Data    interface{} `json:"data,omitempty"`    // 附加错误数据
}
```

### 2. 工具定义结构

```go
// 工具定义 - 描述一个可调用的工具
type Tool struct {
    Name        string `json:"name"`              // 工具唯一名称
    Description string `json:"description"`       // 工具功能描述
    InputSchema struct {                          // 输入参数的JSON Schema
        Type       string                 `json:"type"`       // 通常是 "object"
        Properties map[string]interface{} `json:"properties"` // 参数定义
        Required   []string               `json:"required"`   // 必需参数列表
    } `json:"inputSchema"`
}

// 工具调用参数 - 客户端调用工具时的参数格式
type ToolCallParams struct {
    Name      string                 `json:"name"`      // 要调用的工具名称
    Arguments map[string]interface{} `json:"arguments"` // 工具参数
}

// 工具执行结果 - 工具执行后的返回格式
type ToolResult struct {
    Content []ContentBlock `json:"content"`          // 结果内容块
    IsError bool           `json:"isError,omitempty"` // 是否为错误结果
}

// 内容块 - 结果内容的基本单元
type ContentBlock struct {
    Type string `json:"type"` // 内容类型，通常是 "text"
    Text string `json:"text"` // 文本内容
}
```

### 3. 服务器主体结构

```go
// MCP服务器主体 - 管理所有工具和处理请求
type MCPServer struct {
    tools map[string]Tool // 存储所有注册的工具
}

// 创建新的MCP服务器实例
func NewMCPServer() *MCPServer {
    server := &MCPServer{
        tools: make(map[string]Tool),
    }
    server.registerTools() // 注册所有工具
    return server
}
```

## 🔧 必须实现的核心接口

### 1. HTTP处理接口

```go
// HTTP MCP请求处理器 - 处理所有HTTP请求
func (s *MCPServer) handleHTTPMCP(w http.ResponseWriter, r *http.Request) {
    // 设置响应头
    w.Header().Set("Content-Type", "application/json")
    w.Header().Set("Access-Control-Allow-Origin", "*")
    w.Header().Set("Access-Control-Allow-Methods", "POST, OPTIONS")
    w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

    // 处理OPTIONS预检请求
    if r.Method == "OPTIONS" {
        w.WriteHeader(http.StatusOK)
        return
    }

    // 解析JSON请求
    var msg MCPMessage
    if err := json.NewDecoder(r.Body).Decode(&msg); err != nil {
        // 返回解析错误
        w.WriteHeader(http.StatusBadRequest)
        json.NewEncoder(w).Encode(MCPMessage{
            JSONRPC: "2.0",
            ID:      msg.ID,
            Error: &MCPError{
                Code:    -32700, // Parse error
                Message: "Parse error",
                Data:    err.Error(),
            },
        })
        return
    }

    // 处理消息并返回响应
    response := s.handleMessage(msg)
    if response != nil {
        json.NewEncoder(w).Encode(response)
    }
}
```

### 2. 消息路由接口

```go
// 消息处理路由器 - 根据方法名分发请求
func (s *MCPServer) handleMessage(msg MCPMessage) *MCPMessage {
    switch msg.Method {
    case "initialize":      // 初始化握手
        return s.handleInitialize(msg)
    case "tools/list":      // 获取工具列表
        return s.handleToolsList(msg)
    case "tools/call":      // 调用工具
        return s.handleToolCall(msg)
    case "resources/list":  // 获取资源列表（可选）
        return s.handleResourcesList(msg)
    case "prompts/list":    // 获取提示列表（可选）
        return s.handlePromptsList(msg)
    case "ping":            // 心跳检测
        return &MCPMessage{
            JSONRPC: "2.0",
            ID:      msg.ID,
            Result:  map[string]string{"status": "pong"},
        }
    default:
        return &MCPMessage{
            JSONRPC: "2.0",
            ID:      msg.ID,
            Error: &MCPError{
                Code:    -32601, // Method not found
                Message: "Method not found",
            },
        }
    }
}
```

## 🎯 必须实现的核心方法

### 1. 初始化处理器（必需）

```go
// 处理初始化请求 - 告诉客户端服务器能力
func (s *MCPServer) handleInitialize(msg MCPMessage) *MCPMessage {
    return &MCPMessage{
        JSONRPC: "2.0",
        ID:      msg.ID,
        Result: map[string]interface{}{
            "protocolVersion": "2024-11-05",        // MCP协议版本
            "capabilities": map[string]interface{}{ // 服务器能力声明
                "tools": map[string]interface{}{},  // 支持工具调用
                // "resources": map[string]interface{}{}, // 可选：支持资源访问
                // "prompts": map[string]interface{}{},   // 可选：支持提示模板
            },
            "serverInfo": map[string]interface{}{   // 服务器信息
                "name":    "simple-mcp-server",     // 服务器名称
                "version": "1.0.0",                 // 服务器版本
            },
        },
    }
}
```

### 2. 工具列表处理器（必需）

```go
// 处理工具列表请求 - 告诉客户端有哪些工具可用
func (s *MCPServer) handleToolsList(msg MCPMessage) *MCPMessage {
    tools := make([]Tool, 0, len(s.tools))
    for _, tool := range s.tools {
        tools = append(tools, tool)
    }

    return &MCPMessage{
        JSONRPC: "2.0",
        ID:      msg.ID,
        Result: map[string]interface{}{
            "tools": tools,
        },
    }
}
```

### 3. 工具调用处理器（必需）

```go
// 处理工具调用请求 - 执行具体的工具功能
func (s *MCPServer) handleToolCall(msg MCPMessage) *MCPMessage {
    // 解析调用参数
    paramsBytes, _ := json.Marshal(msg.Params)
    var params ToolCallParams
    if err := json.Unmarshal(paramsBytes, &params); err != nil {
        return &MCPMessage{
            JSONRPC: "2.0",
            ID:      msg.ID,
            Error: &MCPError{
                Code:    -32602, // Invalid params
                Message: "Invalid params",
                Data:    err.Error(),
            },
        }
    }

    // 执行工具
    result, err := s.executeTool(params.Name, params.Arguments)
    if err != nil {
        return &MCPMessage{
            JSONRPC: "2.0",
            ID:      msg.ID,
            Result: ToolResult{
                Content: []ContentBlock{{
                    Type: "text",
                    Text: fmt.Sprintf("工具执行错误: %v", err),
                }},
                IsError: true,
            },
        }
    }

    return &MCPMessage{
        JSONRPC: "2.0",
        ID:      msg.ID,
        Result:  result,
    }
}
```

## 🛠️ 工具开发模式

### 1. 工具注册模式

```go
// 注册所有工具 - 在服务器初始化时调用
func (s *MCPServer) registerTools() {
    // 计算器工具示例
    calculatorTool := Tool{
        Name:        "calculator",
        Description: "执行基本的数学计算（加减乘除、幂运算、开方等）",
        InputSchema: struct {
            Type       string                 `json:"type"`
            Properties map[string]interface{} `json:"properties"`
            Required   []string               `json:"required"`
        }{
            Type: "object",
            Properties: map[string]interface{}{
                "operation": map[string]interface{}{
                    "type":        "string",
                    "description": "要执行的运算类型",
                    "enum":        []string{"add", "subtract", "multiply", "divide", "power", "sqrt"},
                },
                "a": map[string]interface{}{
                    "type":        "number",
                    "description": "第一个数字",
                },
                "b": map[string]interface{}{
                    "type":        "number",
                    "description": "第二个数字（对于开方运算不需要）",
                },
            },
            Required: []string{"operation", "a"},
        },
    }
    s.tools["calculator"] = calculatorTool

    // 继续注册其他工具...
}
```

### 2. 工具执行模式

```go
// 工具执行分发器 - 根据工具名称调用对应实现
func (s *MCPServer) executeTool(toolName string, args map[string]interface{}) (ToolResult, error) {
    switch toolName {
    case "calculator":
        return s.executeCalculator(args)
    case "get_time":
        return s.executeGetTime(args)
    case "generate_uuid":
        return s.executeGenerateUUID(args)
    default:
        return ToolResult{}, fmt.Errorf("unknown tool: %s", toolName)
    }
}

// 具体工具实现示例
func (s *MCPServer) executeCalculator(args map[string]interface{}) (ToolResult, error) {
    // 1. 参数验证
    operation, ok := args["operation"].(string)
    if !ok {
        return ToolResult{}, fmt.Errorf("operation is required")
    }

    a, ok := args["a"].(float64)
    if !ok {
        return ToolResult{}, fmt.Errorf("parameter 'a' must be a number")
    }

    // 2. 业务逻辑处理
    var result float64
    var resultText string

    switch operation {
    case "add":
        b, ok := args["b"].(float64)
        if !ok {
            return ToolResult{}, fmt.Errorf("parameter 'b' is required for addition")
        }
        result = a + b
        resultText = fmt.Sprintf("%.2f + %.2f = %.2f", a, b, result)
    
    case "sqrt":
        if a < 0 {
            return ToolResult{}, fmt.Errorf("cannot calculate square root of negative number")
        }
        result = math.Sqrt(a)
        resultText = fmt.Sprintf("√%.2f = %.2f", a, result)
    
    // ... 其他运算类型
    
    default:
        return ToolResult{}, fmt.Errorf("unknown operation: %s", operation)
    }

    // 3. 返回结果
    return ToolResult{
        Content: []ContentBlock{{
            Type: "text",
            Text: resultText,
        }},
    }, nil
}
```

## 🚀 服务器启动模式

### 1. HTTP服务器设置

```go
func main() {
    // 创建MCP服务器实例
    server := NewMCPServer()

    // 设置HTTP路由
    http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
        // 检查是否是MCP请求
        if r.Header.Get("Content-Type") == "application/json" && r.Method == "POST" {
            server.handleHTTPMCP(w, r)
            return
        }

        // 返回状态页面
        w.Header().Set("Content-Type", "text/html")
        fmt.Fprintf(w, `<!DOCTYPE html>
<html>
<head><title>MCP Server</title></head>
<body>
    <h1>MCP服务器运行中</h1>
    <p>端点: <code>http://localhost:8080/</code></p>
</body>
</html>`)
    })

    // 启动服务器
    port := "8080"
    log.Printf("MCP Server starting on port %s", port)
    log.Printf("HTTP MCP endpoint: http://localhost:%s/", port)
    
    if err := http.ListenAndServe(":"+port, nil); err != nil {
        log.Fatal("Server failed to start:", err)
    }
}
```

## 📝 配置文件格式

### Cursor配置 (.cursor/mcp.json)

```json
{
  "mcpServers": {
    "simple-mcp-server": {
      "url": "http://localhost:8080/"
    }
  }
}
```

## 🎯 开发步骤总结

### 1. 基础结构（必需）
- [ ] 定义 `MCPMessage` 和 `MCPError` 结构
- [ ] 定义 `Tool` 和相关结构
- [ ] 创建 `MCPServer` 主体结构

### 2. 核心接口（必需）
- [ ] 实现 `handleHTTPMCP` HTTP处理器
- [ ] 实现 `handleMessage` 消息路由器
- [ ] 实现 `handleInitialize` 初始化处理器
- [ ] 实现 `handleToolsList` 工具列表处理器
- [ ] 实现 `handleToolCall` 工具调用处理器

### 3. 工具系统（核心功能）
- [ ] 实现 `registerTools` 工具注册方法
- [ ] 实现 `executeTool` 工具执行分发器
- [ ] 为每个工具实现具体执行方法

### 4. 可选功能
- [ ] 实现 `handleResourcesList` 资源列表
- [ ] 实现 `handlePromptsList` 提示列表
- [ ] 添加日志和监控

### 5. 部署配置
- [ ] 创建 `.cursor/mcp.json` 配置文件
- [ ] 设置合适的端口和CORS
- [ ] 添加错误处理和日志

## 🔍 调试技巧

### 1. 日志记录
```go
log.Printf("Received message: %s", msg.Method)
log.Printf("Tool call: %s with args: %+v", params.Name, params.Arguments)
```

### 2. 错误处理
```go
// 总是提供有用的错误信息
if err != nil {
    return ToolResult{
        Content: []ContentBlock{{
            Type: "text",
            Text: fmt.Sprintf("详细错误: %v", err),
        }},
        IsError: true,
    }, nil
}
```

### 3. 测试命令
```bash
# 测试初始化
curl -X POST -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
  http://localhost:8080/

# 测试工具列表
curl -X POST -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' \
  http://localhost:8080/

# 测试工具调用
curl -X POST -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"calculator","arguments":{"operation":"add","a":10,"b":5}}}' \
  http://localhost:8080/
```

## 📚 扩展指南

### 添加新工具的步骤
1. 在 `registerTools` 中定义工具结构
2. 在 `executeTool` 中添加分发逻辑
3. 实现具体的执行方法
4. 添加参数验证和错误处理

### 性能优化建议
1. 使用连接池处理高并发
2. 实现工具结果缓存
3. 添加请求限流
4. 优化JSON序列化性能

这个手册涵盖了基于你的 `main.go` 开发MCP服务器的所有核心要素。按照这个指南，你可以理解和扩展现有代码，或者从零开始构建新的MCP服务器。
