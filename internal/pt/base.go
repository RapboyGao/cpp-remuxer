package pt

import (
	"github.com/pt-muxer-go/internal/media"
	"github.com/sirupsen/logrus"
)

// PTSite PT站点接口
type PTSite interface {
	Name() string
	GenerateFileName(title string, year int, seasonNumber int, episodeNumber int) string
	SortTracks(tracks []media.Track) []media.Track
	FormatMetadata(metadata map[string]interface{}) map[string]interface{}
}

// BasePTSite 基础PT站点实现
type BasePTSite struct {
	Logger *logrus.Logger
}

// NewBasePTSite 创建新的基础PT站点实例
func NewBasePTSite(logger *logrus.Logger) *BasePTSite {
	return &BasePTSite{
		Logger: logger,
	}
}

// Name 返回站点名称
func (b *BasePTSite) Name() string {
	return "base"
}

// GenerateFileName 生成文件名
func (b *BasePTSite) GenerateFileName(title string, year int, seasonNumber int, episodeNumber int) string {
	// 基础实现，返回简单的文件名
	if seasonNumber > 0 && episodeNumber > 0 {
		return "base"
	}
	return title
}

// SortTracks 排序轨道
func (b *BasePTSite) SortTracks(tracks []media.Track) []media.Track {
	// 基础实现，返回原轨道列表
	return tracks
}

// FormatMetadata 格式化元数据
func (b *BasePTSite) FormatMetadata(metadata map[string]interface{}) map[string]interface{} {
	// 基础实现，返回原元数据
	return metadata
}

// PTSiteRegistry PT站点注册表
type PTSiteRegistry struct {
	Sites map[string]PTSite
	Logger *logrus.Logger
}

// NewPTSiteRegistry 创建新的PT站点注册表
func NewPTSiteRegistry(logger *logrus.Logger) *PTSiteRegistry {
	registry := &PTSiteRegistry{
		Sites:  make(map[string]PTSite),
		Logger: logger,
	}

	// 注册默认站点
	registry.RegisterSite(NewBeyondHDSite(logger))
	registry.RegisterSite(NewPTPMuxer(logger))
	registry.RegisterSite(NewBluSite(logger))
	registry.RegisterSite(NewAnimeBytesSite(logger))

	return registry
}

// RegisterSite 注册PT站点
func (r *PTSiteRegistry) RegisterSite(site PTSite) {
	r.Sites[site.Name()] = site
	r.Logger.Infof("Registered PT site: %s", site.Name())
}

// GetSite 获取PT站点
func (r *PTSiteRegistry) GetSite(name string) (PTSite, bool) {
	site, exists := r.Sites[name]
	return site, exists
}

// GetDefaultSite 获取默认PT站点
func (r *PTSiteRegistry) GetDefaultSite() PTSite {
	// 默认返回BeyondHD
	if site, exists := r.Sites["beyondhd"]; exists {
		return site
	}
	// 如果BeyondHD不存在，返回第一个注册的站点
	for _, site := range r.Sites {
		return site
	}
	// 如果没有注册任何站点，返回基础站点
	return NewBasePTSite(r.Logger)
}
