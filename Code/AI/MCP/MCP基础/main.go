package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math"
	"net/http"
	"time"

	"github.com/google/uuid"
)

// MCP 协议的基本消息结构
type MCPMessage struct {
	JSONRPC string      `json:"jsonrpc"`
	ID      interface{} `json:"id,omitempty"`
	Method  string      `json:"method,omitempty"`
	Params  interface{} `json:"params,omitempty"`
	Result  interface{} `json:"result,omitempty"`
	Error   *MCPError   `json:"error,omitempty"`
}

type MCPError struct {
	Code    int         `json:"code"`
	Message string      `json:"message"`
	Data    interface{} `json:"data,omitempty"`
}

// 工具定义结构
type Tool struct {
	Name        string `json:"name"`
	Description string `json:"description"`
	InputSchema struct {
		Type       string                 `json:"type"`
		Properties map[string]interface{} `json:"properties"`
		Required   []string               `json:"required"`
	} `json:"inputSchema"`
}

// 工具调用参数
type ToolCallParams struct {
	Name      string                 `json:"name"`
	Arguments map[string]interface{} `json:"arguments"`
}

// 工具调用结果
type ToolResult struct {
	Content []ContentBlock `json:"content"`
	IsError bool           `json:"isError,omitempty"`
}

type ContentBlock struct {
	Type string `json:"type"`
	Text string `json:"text"`
}

// MCP 服务器
type MCPServer struct {
	tools map[string]Tool
}

func NewMCPServer() *MCPServer {
	server := &MCPServer{
		tools: make(map[string]Tool),
	}

	// 注册内置工具
	server.registerTools()
	return server
}

// 注册工具 - 这里定义了 AI 可以调用的工具
func (s *MCPServer) registerTools() {
	// 计算器工具
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

	// 时间工具
	timeTool := Tool{
		Name:        "get_time",
		Description: "获取当前时间或格式化时间",
		InputSchema: struct {
			Type       string                 `json:"type"`
			Properties map[string]interface{} `json:"properties"`
			Required   []string               `json:"required"`
		}{
			Type: "object",
			Properties: map[string]interface{}{
				"format": map[string]interface{}{
					"type":        "string",
					"description": "时间格式（可选）",
					"default":     "2006-01-02 15:04:05",
				},
				"timezone": map[string]interface{}{
					"type":        "string",
					"description": "时区（可选）",
					"default":     "Local",
				},
			},
			Required: []string{},
		},
	}
	s.tools["get_time"] = timeTool

	// UUID 生成工具
	uuidTool := Tool{
		Name:        "generate_uuid",
		Description: "生成一个新的 UUID",
		InputSchema: struct {
			Type       string                 `json:"type"`
			Properties map[string]interface{} `json:"properties"`
			Required   []string               `json:"required"`
		}{
			Type:       "object",
			Properties: map[string]interface{}{},
			Required:   []string{},
		},
	}
	s.tools["generate_uuid"] = uuidTool
}

// 处理 HTTP MCP 请求
func (s *MCPServer) handleHTTPMCP(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Access-Control-Allow-Methods", "POST, OPTIONS")
	w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

	if r.Method == "OPTIONS" {
		w.WriteHeader(http.StatusOK)
		return
	}

	var msg MCPMessage
	if err := json.NewDecoder(r.Body).Decode(&msg); err != nil {
		w.WriteHeader(http.StatusBadRequest)
		json.NewEncoder(w).Encode(MCPMessage{
			JSONRPC: "2.0",
			ID:      msg.ID,
			Error: &MCPError{
				Code:    -32700,
				Message: "Parse error",
				Data:    err.Error(),
			},
		})
		return
	}

	response := s.handleMessage(msg)
	if response != nil {
		json.NewEncoder(w).Encode(response)
	}
}

// 处理 MCP 消息 - 这是核心的消息路由逻辑
func (s *MCPServer) handleMessage(msg MCPMessage) *MCPMessage {
	switch msg.Method {
	case "initialize":
		return s.handleInitialize(msg)
	case "tools/list":
		return s.handleToolsList(msg)
	case "tools/call":
		return s.handleToolCall(msg)
	case "resources/list":
		return s.handleResourcesList(msg)
	case "prompts/list":
		return s.handlePromptsList(msg)
	case "ping":
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
				Code:    -32601,
				Message: "Method not found",
			},
		}
	}
}

// 初始化处理 - 告诉客户端服务器的能力
func (s *MCPServer) handleInitialize(msg MCPMessage) *MCPMessage {
	return &MCPMessage{
		JSONRPC: "2.0",
		ID:      msg.ID,
		Result: map[string]interface{}{
			"protocolVersion": "2024-11-05",
			"capabilities": map[string]interface{}{
				"tools": map[string]interface{}{},
			},
			"serverInfo": map[string]interface{}{
				"name":    "simple-mcp-server",
				"version": "1.0.0",
			},
		},
	}
}

// 工具列表处理 - 告诉 AI 有哪些工具可用
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

// 资源列表处理 - 暂无资源
func (s *MCPServer) handleResourcesList(msg MCPMessage) *MCPMessage {
	return &MCPMessage{
		JSONRPC: "2.0",
		ID:      msg.ID,
		Result:  map[string]interface{}{"resources": []interface{}{}},
	}
}

// 提示列表处理 - 暂无提示
func (s *MCPServer) handlePromptsList(msg MCPMessage) *MCPMessage {
	return &MCPMessage{
		JSONRPC: "2.0",
		ID:      msg.ID,
		Result:  map[string]interface{}{"prompts": []interface{}{}},
	}
}

// 工具调用处理 - 执行具体的工具函数
func (s *MCPServer) handleToolCall(msg MCPMessage) *MCPMessage {
	paramsBytes, _ := json.Marshal(msg.Params)
	var params ToolCallParams
	if err := json.Unmarshal(paramsBytes, &params); err != nil {
		return &MCPMessage{
			JSONRPC: "2.0",
			ID:      msg.ID,
			Error: &MCPError{
				Code:    -32602,
				Message: "Invalid params",
				Data:    err.Error(),
			},
		}
	}

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

// 执行具体工具 - 这里是实际的工具逻辑
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

// 计算器工具实现
func (s *MCPServer) executeCalculator(args map[string]interface{}) (ToolResult, error) {
	operation, ok := args["operation"].(string)
	if !ok {
		return ToolResult{}, fmt.Errorf("operation is required")
	}

	a, ok := args["a"].(float64)
	if !ok {
		return ToolResult{}, fmt.Errorf("parameter 'a' must be a number")
	}

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
	case "subtract":
		b, ok := args["b"].(float64)
		if !ok {
			return ToolResult{}, fmt.Errorf("parameter 'b' is required for subtraction")
		}
		result = a - b
		resultText = fmt.Sprintf("%.2f - %.2f = %.2f", a, b, result)
	case "multiply":
		b, ok := args["b"].(float64)
		if !ok {
			return ToolResult{}, fmt.Errorf("parameter 'b' is required for multiplication")
		}
		result = a * b
		resultText = fmt.Sprintf("%.2f × %.2f = %.2f", a, b, result)
	case "divide":
		b, ok := args["b"].(float64)
		if !ok {
			return ToolResult{}, fmt.Errorf("parameter 'b' is required for division")
		}
		if b == 0 {
			return ToolResult{}, fmt.Errorf("cannot divide by zero")
		}
		result = a / b
		resultText = fmt.Sprintf("%.2f ÷ %.2f = %.2f", a, b, result)
	case "power":
		b, ok := args["b"].(float64)
		if !ok {
			return ToolResult{}, fmt.Errorf("parameter 'b' is required for power operation")
		}
		result = math.Pow(a, b)
		resultText = fmt.Sprintf("%.2f ^ %.2f = %.2f", a, b, result)
	case "sqrt":
		if a < 0 {
			return ToolResult{}, fmt.Errorf("cannot calculate square root of negative number")
		}
		result = math.Sqrt(a)
		resultText = fmt.Sprintf("√%.2f = %.2f", a, result)
	default:
		return ToolResult{}, fmt.Errorf("unknown operation: %s", operation)
	}

	return ToolResult{
		Content: []ContentBlock{{
			Type: "text",
			Text: resultText,
		}},
	}, nil
}

// 时间工具实现
func (s *MCPServer) executeGetTime(args map[string]interface{}) (ToolResult, error) {
	format := "2006-01-02 15:04:05"
	if f, ok := args["format"].(string); ok && f != "" {
		format = f
	}

	timezone := "Local"
	if tz, ok := args["timezone"].(string); ok && tz != "" {
		timezone = tz
	}

	var loc *time.Location
	var err error
	if timezone == "Local" {
		loc = time.Local
	} else {
		loc, err = time.LoadLocation(timezone)
		if err != nil {
			return ToolResult{}, fmt.Errorf("invalid timezone: %s", timezone)
		}
	}

	now := time.Now().In(loc)
	timeStr := now.Format(format)

	return ToolResult{
		Content: []ContentBlock{{
			Type: "text",
			Text: fmt.Sprintf("当前时间: %s (时区: %s)", timeStr, timezone),
		}},
	}, nil
}

// UUID 生成工具实现
func (s *MCPServer) executeGenerateUUID(args map[string]interface{}) (ToolResult, error) {
	newUUID := uuid.New()
	return ToolResult{
		Content: []ContentBlock{{
			Type: "text",
			Text: fmt.Sprintf("生成的 UUID: %s", newUUID.String()),
		}},
	}, nil
}

func main() {
	server := NewMCPServer()

	// 根路径处理 - 只支持 HTTP MCP 协议
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		// 检查是否是 MCP HTTP 请求
		if r.Header.Get("Content-Type") == "application/json" && r.Method == "POST" {
			server.handleHTTPMCP(w, r)
			return
		}

		// 默认返回状态页面
		w.Header().Set("Content-Type", "text/html")
		fmt.Fprintf(w, `
<!DOCTYPE html>
<html>
<head>
    <title>Simple MCP Server</title>
    <meta charset="UTF-8">
</head>
<body>
    <h1>Simple MCP Server</h1>
    <p>服务器正在运行中...</p>
    <h2>可用工具:</h2>
    <ul>
        <li><strong>calculator</strong> - 执行基本数学运算</li>
        <li><strong>get_time</strong> - 获取当前时间</li>
        <li><strong>generate_uuid</strong> - 生成 UUID</li>
    </ul>
    <h2>连接信息:</h2>
    <p>HTTP MCP 端点: <code>http://localhost:8080/</code></p>
</body>
</html>
		`)
	})

	port := "8080"
	log.Printf("MCP Server starting on port %s", port)
	log.Printf("HTTP MCP endpoint: http://localhost:%s/", port)
	log.Printf("Status page: http://localhost:%s/", port)

	if err := http.ListenAndServe(":"+port, nil); err != nil {
		log.Fatal("Server failed to start:", err)
	}
}
