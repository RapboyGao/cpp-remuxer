package demux

import (
	"os"
	"path/filepath"

	"github.com/pt-muxer-go/internal/app"
	"github.com/pt-muxer-go/internal/media"
	"github.com/pt-muxer-go/internal/tools"
	"github.com/sirupsen/logrus"
)

// Demuxer 解复用器接口
type Demuxer interface {
	Demux() error
	Cleanup() error
}

// DemuxOptions 解复用选项
type DemuxOptions struct {
	BDMVPath        string
	OutputDir       string
	MediaInfo       *media.PlaylistInfo
	IsMovie         bool
	SeasonNumber    int
	EpisodeNumber   int
}

// BaseDemuxer 基础解复用器
type BaseDemuxer struct {
	App         *app.App
	BDMV        *media.BDMV
	Options     *DemuxOptions
	OutputDir   string
	DemuxFolder string
	Success     bool
	Logger      *logrus.Logger
	CmdRunner   *tools.CommandRunner
}

// NewBaseDemuxer 创建新的基础解复用器
func NewBaseDemuxer(app *app.App, bdmv *media.BDMV, options *DemuxOptions) (*BaseDemuxer, error) {
	// 创建输出目录
	outputDir := options.OutputDir
	if outputDir == "" {
		outputDir = app.Config.General.DefaultOutputDir
	}

	os.MkdirAll(outputDir, 0755)

	// 创建解复用文件夹
	demuxFolder := filepath.Join(outputDir, "demux_temp")
	os.MkdirAll(demuxFolder, 0755)

	demuxer := &BaseDemuxer{
		App:         app,
		BDMV:        bdmv,
		Options:     options,
		OutputDir:   outputDir,
		DemuxFolder: demuxFolder,
		Success:     false,
		Logger:      app.Logger,
		CmdRunner:   tools.NewCommandRunner(app.Logger),
	}

	return demuxer, nil
}

// Cleanup 清理临时文件
func (d *BaseDemuxer) Cleanup() error {
	if !d.Success && d.DemuxFolder != "" {
		// 如果解复用失败，删除临时文件夹
		d.Logger.Infof("Deleting failed demux folder: %s", d.DemuxFolder)
		return os.RemoveAll(d.DemuxFolder)
	}
	return nil
}

// RunEAC3to 运行eac3to工具
func (d *BaseDemuxer) RunEAC3to(args ...string) error {
	// 实现eac3to调用逻辑
	_, err := d.CmdRunner.RunCommand(d.App.GetToolPath("eac3to"), args...)
	return err
}

// RunDGDemux 运行dgdemux工具
func (d *BaseDemuxer) RunDGDemux(args ...string) error {
	// 实现dgdemux调用逻辑
	_, err := d.CmdRunner.RunCommand(d.App.GetToolPath("dgdemux"), args...)
	return err
}
