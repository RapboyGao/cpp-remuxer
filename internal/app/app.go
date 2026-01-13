package app

import (
	"os"
	"path/filepath"

	"github.com/sirupsen/logrus"
)

// App 应用核心结构体
type App struct {
	Config *Config
	Logger *logrus.Logger
}

// NewApp 创建新的应用实例
func NewApp() (*App, error) {
	// 初始化日志
	logger := logrus.New()
	logger.SetFormatter(&logrus.TextFormatter{
		FullTimestamp: true,
	})
	logger.SetOutput(os.Stdout)

	// 加载配置
	config, err := LoadConfig()
	if err != nil {
		logger.Errorf("Failed to load config: %v", err)
		return nil, err
	}

	// 设置日志级别
	level, err := logrus.ParseLevel(config.General.LogLevel)
	if err != nil {
		logger.Errorf("Invalid log level: %s, using default info", config.General.LogLevel)
		level = logrus.InfoLevel
	}
	logger.SetLevel(level)

	app := &App{
		Config: config,
		Logger: logger,
	}

	// 验证工具路径
	if err := app.validateToolPaths(); err != nil {
		logger.Errorf("Failed to validate tool paths: %v", err)
		return nil, err
	}

	return app, nil
}

// validateToolPaths 验证外部工具路径
func (a *App) validateToolPaths() error {
	// 检查默认工具路径
	toolPaths := map[string]*string{
		"ffmpeg":   &a.Config.Tools.FFmpegPath,
		"mkvmerge": &a.Config.Tools.MKVMergePath,
		"eac3to":   &a.Config.Tools.EAC3toPath,
		"dgdemux":  &a.Config.Tools.DGDemuxPath,
	}

	for toolName, toolPath := range toolPaths {
		if *toolPath == "" {
			// 尝试自动查找工具
			foundPath, err := findToolPath(toolName)
			if err != nil {
				a.Logger.Warnf("Failed to find %s path: %v, will try to use system PATH", toolName, err)
				continue
			}
			a.Logger.Infof("Found %s at: %s", toolName, foundPath)
			*toolPath = foundPath
		} else {
			// 验证工具路径是否存在
			if _, err := os.Stat(*toolPath); os.IsNotExist(err) {
				a.Logger.Warnf("Configured %s path does not exist: %s", toolName, *toolPath)
			}
		}
	}

	return nil
}

// findToolPath 自动查找工具路径
func findToolPath(toolName string) (string, error) {
	// 获取应用根目录
	// 首先尝试从可执行文件路径获取
	execPath, err := os.Executable()
	var appRoot string
	if err != nil {
		// 如果失败，回退到当前工作目录
		appRoot, err = os.Getwd()
		if err != nil {
			return "", err
		}
	} else {
		// 获取应用根目录（假设可执行文件在cmd/ptmuxer目录下）
		appRoot = filepath.Dir(filepath.Dir(filepath.Dir(execPath)))
	}

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
		return toolPath, nil
	}

	// 如果没有找到，尝试检查系统PATH
	if isWindows {
		return filepath.Abs(toolName+".exe")
	}
	return filepath.Abs(toolName)
}

// GetToolPath 获取工具路径
func (a *App) GetToolPath(toolName string) string {
	switch toolName {
	case "ffmpeg":
		return a.Config.Tools.FFmpegPath
	case "mkvmerge":
		return a.Config.Tools.MKVMergePath
	case "eac3to":
		return a.Config.Tools.EAC3toPath
	case "dgdemux":
		return a.Config.Tools.DGDemuxPath
	default:
		return toolName
	}
}
