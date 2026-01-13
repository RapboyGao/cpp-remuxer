package main

import (
	"image"
	"image/color"
	"image/png"
	"os"
)

func main() {
	// 创建一个简单的128x128红色方块图标
	width, height := 128, 128
	img := image.NewRGBA(image.Rect(0, 0, width, height))

	// 填充红色背景
	red := color.RGBA{255, 0, 0, 255}
	for x := 0; x < width; x++ {
		for y := 0; y < height; y++ {
			img.Set(x, y, red)
		}
	}

	// 保存为PNG文件
	file, err := os.Create("Icon.png")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	if err := png.Encode(file, img); err != nil {
		panic(err)
	}

	println("Icon.png created successfully")
}
