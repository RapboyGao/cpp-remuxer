package media

import (
	"os"
	"path/filepath"
	"strings"

	"github.com/sirupsen/logrus"
)

// BDMV 表示BDMV目录结构
type BDMV struct {
	RootPath      string
	PlaylistFiles []string
	StreamFiles   []string
	ClipInfoFiles []string
	Logger        *logrus.Logger
}

// NewBDMV 创建新的BDMV实例
func NewBDMV(rootPath string, logger *logrus.Logger) (*BDMV, error) {
	// 检查BDMV目录是否存在
	bdmvPath := filepath.Join(rootPath, "BDMV")
	if _, err := os.Stat(bdmvPath); os.IsNotExist(err) {
		return nil, err
	}

	bdmv := &BDMV{
		RootPath: rootPath,
		Logger:   logger,
	}

	// 扫描BDMV目录结构
	if err := bdmv.scanDirectory(); err != nil {
		return nil, err
	}

	return bdmv, nil
}

// scanDirectory 扫描BDMV目录结构
func (b *BDMV) scanDirectory() error {
	bdmvPath := filepath.Join(b.RootPath, "BDMV")

	// 扫描PLAYLIST目录
	playlistPath := filepath.Join(bdmvPath, "PLAYLIST")
	if _, err := os.Stat(playlistPath); err == nil {
		files, err := os.ReadDir(playlistPath)
		if err != nil {
			return err
		}

		for _, file := range files {
			if !file.IsDir() && strings.HasSuffix(file.Name(), ".mpls") {
				b.PlaylistFiles = append(b.PlaylistFiles, filepath.Join(playlistPath, file.Name()))
			}
		}
	}

	// 扫描STREAM目录
	streamPath := filepath.Join(bdmvPath, "STREAM")
	if _, err := os.Stat(streamPath); err == nil {
		files, err := os.ReadDir(streamPath)
		if err != nil {
			return err
		}

		for _, file := range files {
			if !file.IsDir() && strings.HasSuffix(file.Name(), ".m2ts") {
				b.StreamFiles = append(b.StreamFiles, filepath.Join(streamPath, file.Name()))
			}
		}
	}

	// 扫描CLIPINF目录
	clipInfoPath := filepath.Join(bdmvPath, "CLIPINF")
	if _, err := os.Stat(clipInfoPath); err == nil {
		files, err := os.ReadDir(clipInfoPath)
		if err != nil {
			return err
		}

		for _, file := range files {
			if !file.IsDir() && strings.HasSuffix(file.Name(), ".clpi") {
				b.ClipInfoFiles = append(b.ClipInfoFiles, filepath.Join(clipInfoPath, file.Name()))
			}
		}
	}

	b.Logger.Infof("Found %d playlist files, %d stream files, %d clip info files", 
		len(b.PlaylistFiles), len(b.StreamFiles), len(b.ClipInfoFiles))

	return nil
}

// GetMainPlaylist 获取主播放列表
func (b *BDMV) GetMainPlaylist() (string, error) {
	// 通常最大的mpls文件是主播放列表
	var mainPlaylist string
	var maxSize int64

	for _, playlist := range b.PlaylistFiles {
		info, err := os.Stat(playlist)
		if err != nil {
			b.Logger.Errorf("Failed to get file info for %s: %v", playlist, err)
			continue
		}

		if info.Size() > maxSize {
			maxSize = info.Size()
			mainPlaylist = playlist
		}
	}

	if mainPlaylist == "" {
		return "", os.ErrNotExist
	}

	b.Logger.Infof("Main playlist: %s (size: %d bytes)", mainPlaylist, maxSize)
	return mainPlaylist, nil
}

// Track 表示音视频轨道信息
type Track struct {
	ID          int
	Type        string // video, audio, subtitle
	Codec       string
	Language    string
	Title       string
	Resolution  string
	FrameRate   float64
	BitRate     int
	Channels    int
	SampleRate  int
	IsDefault   bool
	IsForced    bool
	IsExternal  bool
	FilePath    string
}

// PlaylistInfo 表示播放列表信息
type PlaylistInfo struct {
	Duration    int64 // in milliseconds
	Tracks      []Track
	StreamFiles []string
}

// ParsePlaylist 解析播放列表文件
func (b *BDMV) ParsePlaylist(playlistPath string) (*PlaylistInfo, error) {
	// 这里需要实现实际的mpls文件解析
	// 目前只是返回一个空的PlaylistInfo
	b.Logger.Infof("Parsing playlist: %s", playlistPath)

	// 简单实现，后续需要完善
	info := &PlaylistInfo{
		Duration: 0,
		Tracks:   []Track{},
		StreamFiles: []string{},
	}

	return info, nil
}
