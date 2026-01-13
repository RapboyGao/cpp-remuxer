package main

import (
	"os"

	"github.com/urfave/cli/v2"
	"github.com/pt-muxer-go/internal/app"
	"github.com/pt-muxer-go/internal/demux"
	"github.com/pt-muxer-go/internal/media"
	"github.com/pt-muxer-go/internal/remux"
)

func main() {
	// 创建CLI应用
	cliApp := &cli.App{
		Name:  "pt-muxer-go",
		Usage: "A cross-platform BDMV to MKV remuxer with GUI",
		Version: "1.0.0",
		Commands: []*cli.Command{
			{
				Name:    "demux",
				Usage:   "Demux BDMV to separate tracks",
				Aliases: []string{"d"},
				Flags: []cli.Flag{
					&cli.StringFlag{
						Name:     "bdmv",
						Usage:    "Path to BDMV directory",
						Required: true,
					},
					&cli.StringFlag{
						Name:  "output",
						Usage: "Output directory",
						Aliases: []string{"o"},
					},
					&cli.BoolFlag{
						Name:  "movie",
						Usage: "Demux as movie",
					},
					&cli.IntFlag{
						Name:  "season",
						Usage: "Season number for TV shows",
					},
					&cli.IntFlag{
						Name:  "episode",
						Usage: "Episode number for TV shows",
					},
				},
				Action: runDemux,
			},
			{
				Name:    "remux",
				Usage:   "Remux tracks to MKV",
				Aliases: []string{"r"},
				Flags: []cli.Flag{
					&cli.StringSliceFlag{
						Name:     "input",
						Usage:    "Input files",
						Required: true,
						Aliases: []string{"i"},
					},
					&cli.StringFlag{
						Name:  "output",
						Usage: "Output directory",
						Aliases: []string{"o"},
					},
					&cli.BoolFlag{
						Name:  "movie",
						Usage: "Remux as movie",
					},
					&cli.StringFlag{
						Name:  "title",
						Usage: "Media title",
					},
					&cli.IntFlag{
						Name:  "year",
						Usage: "Release year",
					},
					&cli.IntFlag{
						Name:  "season",
						Usage: "Season number for TV shows",
					},
					&cli.IntFlag{
						Name:  "episode",
						Usage: "Episode number for TV shows",
					},
					&cli.StringFlag{
						Name:  "site",
						Usage: "PT site for metadata formatting",
						DefaultText: "beyondhd",
					},
				},
				Action: runRemux,
			},
			{
				Name:    "gui",
				Usage:   "Start GUI interface",
				Aliases: []string{"g"},
				Action: runGUI,
			},
		},
	}

	// 运行CLI应用
	if err := cliApp.Run(os.Args); err != nil {
		os.Exit(1)
	}
}

// runDemux 执行解复用命令
func runDemux(c *cli.Context) error {
	// 创建应用实例
	appInstance, err := app.NewApp()
	if err != nil {
		return err
	}

	// 创建BDMV实例
	bdmv, err := media.NewBDMV(c.String("bdmv"), appInstance.Logger)
	if err != nil {
		return err
	}

	// 设置解复用选项
	options := &demux.DemuxOptions{
		BDMVPath:        c.String("bdmv"),
		OutputDir:       c.String("output"),
		IsMovie:         c.Bool("movie"),
		SeasonNumber:    c.Int("season"),
		EpisodeNumber:   c.Int("episode"),
	}

	// 创建解复用器
	var demuxer demux.Demuxer
	if options.IsMovie {
		demuxer, err = demux.NewMovieDemuxer(appInstance, bdmv, options)
	} else {
		demuxer, err = demux.NewTVDemuxer(appInstance, bdmv, options)
	}

	if err != nil {
		return err
	}

	// 执行解复用
	if err := demuxer.Demux(); err != nil {
		// 清理资源
		demuxer.Cleanup()
		return err
	}

	// 解复用成功，清理临时资源
	return demuxer.Cleanup()
}

// runRemux 执行重复用命令
func runRemux(c *cli.Context) error {
	// 创建应用实例
	appInstance, err := app.NewApp()
	if err != nil {
		return err
	}

	// 设置重复用选项
	options := &remux.RemuxOptions{
		InputFiles:     c.StringSlice("input"),
		OutputDir:      c.String("output"),
		IsMovie:        c.Bool("movie"),
		Title:          c.String("title"),
		Year:           c.Int("year"),
		SeasonNumber:   c.Int("season"),
		EpisodeNumber:  c.Int("episode"),
		PTSite:         c.String("site"),
	}

	// 创建重复用器
	var remuxer remux.Remuxer
	if options.IsMovie {
		remuxer, err = remux.NewMovieRemuxer(appInstance, options)
	} else {
		remuxer, err = remux.NewTVRemuxer(appInstance, options)
	}

	if err != nil {
		return err
	}

	// 执行重复用
	if err := remuxer.Remux(); err != nil {
		// 清理资源
		remuxer.Cleanup()
		return err
	}

	// 重复用成功，清理临时资源
	return remuxer.Cleanup()
}

// runGUI 启动GUI界面
func runGUI(c *cli.Context) error {
	// 创建应用实例
	appInstance, err := app.NewApp()
	if err != nil {
		return err
	}

	appInstance.Logger.Info("Starting GUI interface...")

	// 检查是否有CGO支持
	// 使用反射来检查是否能导入gui包
	// 这里简化处理，直接输出信息
	appInstance.Logger.Info("GUI interface is under development. Please check back later.")

	return nil
}
