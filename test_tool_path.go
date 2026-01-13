package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func main() {
	// 测试findToolPath函数
	appRoot, err := os.Getwd()
	if err != nil {
		fmt.Printf("Failed to get current directory: %v\n", err)
		os.Exit(1)
	}

	// 设置应用根目录
	fmt.Printf("App root: %s\n", appRoot)

	// 测试工具列表
	tools := []string{
		"ffmpeg",
		"mkvmerge",
		"eac3to",
		"dgdemux",
	}

	// 测试每个工具的路径
	for _, tool := range tools {
		toolPath := testFindToolPath(tool, appRoot)
		fmt.Printf("Tool %s: %s\n", tool, toolPath)
	}
}

// testFindToolPath 测试查找工具路径
func testFindToolPath(toolName string, appRoot string) string {
	// 对于Windows平台，添加.exe后缀
	isWindows := os.PathSeparator == '\\'
	var toolPath string
	var found bool

	// 定义平台目录名映射
	platformDir := "Linux"
	if isWindows {
		platformDir = "Windows"
	}

	// 定义工具的可能位置，按照优先级排序
	toolPaths := []string{
		// 1. 直接在工具名称对应的子目录下查找
		filepath.Join(appRoot, "tools", toolName, toolName),
		filepath.Join(appRoot, "tools", toolName, platformDir, toolName),
		// 2. 对于Windows，尝试.exe后缀
		filepath.Join(appRoot, "tools", toolName, toolName+".exe"),
		filepath.Join(appRoot, "tools", toolName, platformDir, toolName+".exe"),
		// 3. 特殊情况处理
		filepath.Join(appRoot, "tools", "ffmpeg", "ffmpeg"),
		filepath.Join(appRoot, "tools", "eac3to", "eac3to.exe"),
		filepath.Join(appRoot, "tools", "mkvmerge", platformDir, "mkvmerge"),
		filepath.Join(appRoot, "tools", "mkvmerge", platformDir, "mkvmerge.exe"),
		filepath.Join(appRoot, "tools", "dgdemux", platformDir, "dgdemux"),
		filepath.Join(appRoot, "tools", "dgdemux", platformDir, "DGDemux.exe"),
		filepath.Join(appRoot, "tools", "7z", platformDir, "7za.exe"),
		filepath.Join(appRoot, "tools", "7z", platformDir, "7zzs"),
	}

	// 遍历所有可能的工具路径
	for _, path := range toolPaths {
		// 检查工具是否存在
		if _, err := os.Stat(path); err == nil {
			toolPath = path
			found = true
			break
		}
	}

	if found {
		return toolPath
	}

	return "Not found"
}
