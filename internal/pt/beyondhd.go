package pt

import (
	"fmt"
	"sort"

	"github.com/pt-muxer-go/internal/media"
	"github.com/sirupsen/logrus"
)

// BeyondHDSite BeyondHD站点实现
type BeyondHDSite struct {
	*BasePTSite
}

// NewBeyondHDSite 创建新的BeyondHD站点实例
func NewBeyondHDSite(logger *logrus.Logger) *BeyondHDSite {
	return &BeyondHDSite{
		BasePTSite: NewBasePTSite(logger),
	}
}

// Name 返回站点名称
func (b *BeyondHDSite) Name() string {
	return "beyondhd"
}

// GenerateFileName 生成BeyondHD格式的文件名
func (b *BeyondHDSite) GenerateFileName(title string, year int, seasonNumber int, episodeNumber int) string {
	if seasonNumber > 0 && episodeNumber > 0 {
		// 电视剧格式: Title SXXEXX
		return fmt.Sprintf("%s S%02dE%02d", title, seasonNumber, episodeNumber)
	}
	// 电影格式: Title (Year)
	return fmt.Sprintf("%s (%d)", title, year)
}

// SortTracks 按照BeyondHD规则排序轨道
func (b *BeyondHDSite) SortTracks(tracks []media.Track) []media.Track {
	// 复制轨道列表以避免修改原列表
	sortedTracks := make([]media.Track, len(tracks))
	copy(sortedTracks, tracks)

	// 定义轨道优先级
	languagePriority := map[string]int{
		"eng": 1,
		"jpn": 2,
		"fra": 3,
		"deu": 4,
		"spa": 5,
		"ita": 6,
		"por": 7,
		"rus": 8,
	}

	codecPriority := map[string]int{
		"h264":  1,
		"h265":  2,
		"mpeg2": 3,
		"vc1":   4,
	}

	typePriority := map[string]int{
		"video":    1,
		"audio":    2,
		"subtitle": 3,
	}

	// 排序轨道
	sort.Slice(sortedTracks, func(i, j int) bool {
		// 按类型排序
		if typePriority[sortedTracks[i].Type] != typePriority[sortedTracks[j].Type] {
			return typePriority[sortedTracks[i].Type] < typePriority[sortedTracks[j].Type]
		}

		// 视频轨道按编码排序
		if sortedTracks[i].Type == "video" {
			if codecPriority[sortedTracks[i].Codec] != codecPriority[sortedTracks[j].Codec] {
				return codecPriority[sortedTracks[i].Codec] < codecPriority[sortedTracks[j].Codec]
			}
			// 按分辨率排序
			return sortedTracks[i].Resolution > sortedTracks[j].Resolution
		}

		// 音频轨道按语言和编码排序
		if sortedTracks[i].Type == "audio" {
			// 按语言优先级排序
			if languagePriority[sortedTracks[i].Language] != languagePriority[sortedTracks[j].Language] {
				return languagePriority[sortedTracks[i].Language] < languagePriority[sortedTracks[j].Language]
			}
			// 按声道数排序
			if sortedTracks[i].Channels != sortedTracks[j].Channels {
				return sortedTracks[i].Channels > sortedTracks[j].Channels
			}
			// 按编码优先级排序
			return sortedTracks[i].Codec < sortedTracks[j].Codec
		}

		// 字幕轨道按语言排序
		if sortedTracks[i].Type == "subtitle" {
			// 按语言优先级排序
			if languagePriority[sortedTracks[i].Language] != languagePriority[sortedTracks[j].Language] {
				return languagePriority[sortedTracks[i].Language] < languagePriority[sortedTracks[j].Language]
			}
			// 强制字幕优先
			if sortedTracks[i].IsForced != sortedTracks[j].IsForced {
				return sortedTracks[i].IsForced
			}
			// 默认字幕优先
			if sortedTracks[i].IsDefault != sortedTracks[j].IsDefault {
				return sortedTracks[i].IsDefault
			}
			return sortedTracks[i].Codec < sortedTracks[j].Codec
		}

		return sortedTracks[i].ID < sortedTracks[j].ID
	})

	return sortedTracks
}

// FormatMetadata 格式化BeyondHD元数据
func (b *BeyondHDSite) FormatMetadata(metadata map[string]interface{}) map[string]interface{} {
	// 实现BeyondHD元数据格式化
	return metadata
}
