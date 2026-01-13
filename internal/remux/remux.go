package remux

import (
	"os"
	"path/filepath"

	"github.com/pt-muxer-go/internal/app"
	"github.com/pt-muxer-go/internal/media"
	"github.com/pt-muxer-go/internal/tools"
	"github.com/sirupsen/logrus"
)

// Remuxer 重复用器接口
type Remuxer interface {
	Remux() error
	Cleanup() error
}

// RemuxOptions 重复用选项
type RemuxOptions struct {
	InputFiles       []string
	OutputDir        string
	MediaInfo        *media.PlaylistInfo
	IsMovie          bool
	Title            string
	Year             int
	SeasonNumber     int
	EpisodeNumber    int
	PTSite           string
}

// BaseRemuxer 基础重复用器
type BaseRemuxer struct {
	App         *app.App
	Options     *RemuxOptions
	OutputDir   string
	RemuxFolder string
	Success     bool
	Logger      *logrus.Logger
	CmdRunner   *tools.CommandRunner
}

// NewBaseRemuxer 创建新的基础重复用器
func NewBaseRemuxer(app *app.App, options *RemuxOptions) (*BaseRemuxer, error) {
	// 创建输出目录
	outputDir := options.OutputDir
	if outputDir == "" {
		outputDir = app.Config.General.DefaultOutputDir
	}

	os.MkdirAll(outputDir, 0755)

	// 创建重复用文件夹
	remuxFolder := filepath.Join(outputDir, "remux_temp")
	os.MkdirAll(remuxFolder, 0755)

	remuxer := &BaseRemuxer{
		App:         app,
		Options:     options,
		OutputDir:   outputDir,
		RemuxFolder: remuxFolder,
		Success:     false,
		Logger:      app.Logger,
		CmdRunner:   tools.NewCommandRunner(app.Logger),
	}

	return remuxer, nil
}

// Cleanup 清理临时文件
func (r *BaseRemuxer) Cleanup() error {
	if !r.Success && r.RemuxFolder != "" {
		// 如果重复用失败，删除临时文件夹
		r.Logger.Infof("Deleting failed remux folder: %s", r.RemuxFolder)
		return os.RemoveAll(r.RemuxFolder)
	}
	return nil
}

// RunMKVMerge 运行mkvmerge工具
func (r *BaseRemuxer) RunMKVMerge(args ...string) error {
	// 实现mkvmerge调用逻辑
	_, err := r.CmdRunner.RunCommand(r.App.GetToolPath("mkvmerge"), args...)
	return err
}

// GenerateOutputFileName 生成输出文件名
func (r *BaseRemuxer) GenerateOutputFileName() string {
	// 根据媒体类型生成不同的文件名
	if r.Options.IsMovie {
		return r.generateMovieFileName()
	}
	return r.generateTVFileName()
}

// generateMovieFileName 生成电影文件名
func (r *BaseRemuxer) generateMovieFileName() string {
	// 简单实现，后续需要根据PT站点规则完善
	return r.Options.Title
}

// generateTVFileName 生成电视剧文件名
func (r *BaseRemuxer) generateTVFileName() string {
	// 简单实现，后续需要根据PT站点规则完善
	return r.Options.Title
}
