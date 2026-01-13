package gui

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"github.com/pt-muxer-go/internal/app"
)

// MainWindow 主窗口
type MainWindow struct {
	App     fyne.App
	Window  fyne.Window
	AppCore *app.App
}

// NewMainWindow 创建新的主窗口
func NewMainWindow(appCore *app.App) *MainWindow {
	fyneApp := app.New()
	window := fyneApp.NewWindow("PT-Muxer Go")
	window.Resize(fyne.NewSize(800, 600))

	mainWindow := &MainWindow{
		App:     fyneApp,
		Window:  window,
		AppCore: appCore,
	}

	// 设置窗口内容
	mainWindow.SetContent()

	return mainWindow
}

// SetContent 设置窗口内容
func (mw *MainWindow) SetContent() {
	// 创建欢迎文本
	welcomeText := widget.NewLabel("Welcome to PT-Muxer Go")
	welcomeText.TextStyle = fyne.TextStyle{
		Bold:  true,
		Size:  24,
	}

	// 创建功能按钮
	demuxButton := widget.NewButton("Demux BDMV", func() {
		mw.openDemuxWindow()
	})
	demuxButton.Importance = widget.HighImportance

	remuxButton := widget.NewButton("Remux to MKV", func() {
		mw.openRemuxWindow()
	})
	remuxButton.Importance = widget.HighImportance

	settingsButton := widget.NewButton("Settings", func() {
		mw.openSettingsWindow()
	})

	// 创建按钮容器
	buttons := container.NewVBox(
		demuxButton,
		remuxButton,
		settingsButton,
	)
	buttons.Alignment = fyne.AlignCenter
	buttons.Spacing = 20

	// 创建主容器
	content := container.NewVBox(
		welcomeText,
		buttons,
	)
	content.Alignment = fyne.AlignCenter
	content.Spacing = 40

	// 设置窗口内容
	mw.Window.SetContent(content)
}

// Show 显示窗口
func (mw *MainWindow) Show() {
	mw.Window.ShowAndRun()
}

// openDemuxWindow 打开解复用窗口
func (mw *MainWindow) openDemuxWindow() {
	mw.AppCore.Logger.Info("Opening demux window")
	// 这里需要实现解复用窗口
}

// openRemuxWindow 打开重复用窗口
func (mw *MainWindow) openRemuxWindow() {
	mw.AppCore.Logger.Info("Opening remux window")
	// 这里需要实现重复用窗口
}

// openSettingsWindow 打开设置窗口
func (mw *MainWindow) openSettingsWindow() {
	mw.AppCore.Logger.Info("Opening settings window")
	// 这里需要实现设置窗口
}
