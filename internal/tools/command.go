package tools

import (
	"bytes"
	"context"
	"io"
	"os/exec"
	"time"

	"github.com/sirupsen/logrus"
)

// CommandRunner 命令执行器
type CommandRunner struct {
	Logger *logrus.Logger
}

// NewCommandRunner 创建新的命令执行器
func NewCommandRunner(logger *logrus.Logger) *CommandRunner {
	return &CommandRunner{
		Logger: logger,
	}
}

// RunCommand 运行外部命令
func (cr *CommandRunner) RunCommand(cmdPath string, args ...string) (string, error) {
	cr.Logger.Infof("Running command: %s %v", cmdPath, args)

	// 创建命令上下文，设置超时
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Hour)
	defer cancel()

	// 创建命令
	cmd := exec.CommandContext(ctx, cmdPath, args...)

	// 捕获输出
	var stdout bytes.Buffer
	var stderr bytes.Buffer
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr

	// 运行命令
	err := cmd.Run()
	output := stdout.String()
	stderrOutput := stderr.String()

	if stderrOutput != "" {
		cr.Logger.Debugf("Command stderr: %s", stderrOutput)
	}

	if err != nil {
		cr.Logger.Errorf("Command failed: %v, stderr: %s", err, stderrOutput)
		return "", err
	}

	cr.Logger.Debugf("Command output: %s", output)
	return output, nil
}

// RunCommandWithCallback 运行外部命令并带有回调
func (cr *CommandRunner) RunCommandWithCallback(cmdPath string, callback func(string), args ...string) error {
	cr.Logger.Infof("Running command with callback: %s %v", cmdPath, args)

	// 创建命令
	cmd := exec.Command(cmdPath, args...)

	// 捕获输出
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		cr.Logger.Errorf("Failed to get stdout pipe: %v", err)
		return err
	}

	stderr, err := cmd.StderrPipe()
	if err != nil {
		cr.Logger.Errorf("Failed to get stderr pipe: %v", err)
		return err
	}

	// 启动命令
	if err := cmd.Start(); err != nil {
		cr.Logger.Errorf("Failed to start command: %v", err)
		return err
	}

	// 处理输出
	go cr.handleOutput(stdout, callback)
	go cr.handleOutput(stderr, callback)

	// 等待命令完成
	if err := cmd.Wait(); err != nil {
		cr.Logger.Errorf("Command failed: %v", err)
		return err
	}

	return nil
}

// handleOutput 处理命令输出
func (cr *CommandRunner) handleOutput(output io.Reader, callback func(string)) {
	buffer := make([]byte, 1024)
	for {
		n, err := output.Read(buffer)
		if n > 0 {
			callback(string(buffer[:n]))
		}
		if err != nil {
			break
		}
	}
}
