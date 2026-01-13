package app

import (
	"os"
	"path/filepath"

	"github.com/sirupsen/logrus"
	"github.com/spf13/viper"
)

// Config 应用配置结构体
type Config struct {
	General struct {
		DefaultOutputDir string `mapstructure:"default_output_dir"`
		LogLevel         string `mapstructure:"log_level"`
	}
	Tools struct {
		FFmpegPath   string `mapstructure:"ffmpeg_path"`
		MKVMergePath string `mapstructure:"mkvmerge_path"`
		EAC3toPath   string `mapstructure:"eac3to_path"`
		DGDemuxPath  string `mapstructure:"dgdemux_path"`
	}
	PT struct {
		DefaultSite string `mapstructure:"default_site"`
	}
}

// LoadConfig 加载配置文件
func LoadConfig() (*Config, error) {
	var config Config

	// 设置配置文件路径
	configDir, err := os.UserConfigDir()
	if err != nil {
		logrus.Errorf("Failed to get user config directory: %v", err)
		return nil, err
	}

	configPath := filepath.Join(configDir, "pt-muxer-go")
	os.MkdirAll(configPath, 0755)

	// 配置viper
	viper.SetConfigName("config")
	viper.SetConfigType("yaml")
	viper.AddConfigPath(configPath)
	viper.AddConfigPath(".")

	// 设置默认值
	setDefaults()

	// 读取配置文件
	if err := viper.ReadInConfig(); err != nil {
		if _, ok := err.(viper.ConfigFileNotFoundError); ok {
			// 配置文件不存在，创建默认配置
			if err := viper.SafeWriteConfigAs(filepath.Join(configPath, "config.yaml")); err != nil {
				logrus.Errorf("Failed to write default config: %v", err)
				return nil, err
			}
		} else {
			// 配置文件存在但读取失败
			logrus.Errorf("Failed to read config: %v", err)
			return nil, err
		}
	}

	// 解析配置
	if err := viper.Unmarshal(&config); err != nil {
		logrus.Errorf("Failed to unmarshal config: %v", err)
		return nil, err
	}

	return &config, nil
}

// 设置默认配置值
func setDefaults() {
	viper.SetDefault("general.default_output_dir", "./output")
	viper.SetDefault("general.log_level", "info")

	// 工具路径默认为空，后续会自动查找
	viper.SetDefault("tools.ffmpeg_path", "")
	viper.SetDefault("tools.mkvmerge_path", "")
	viper.SetDefault("tools.eac3to_path", "")
	viper.SetDefault("tools.dgdemux_path", "")

	viper.SetDefault("pt.default_site", "beyondhd")
}
