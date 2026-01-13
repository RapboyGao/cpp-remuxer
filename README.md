# Blu-ray Remux to MKV CLI Tool

A cross-platform C++ command-line tool to remux Blu-ray main playlist to MKV format without re-encoding.

## Features

- Remux Blu-ray main playlist to MKV format
- Preserves original video, audio, and subtitle streams
- Supports automatic main playlist detection
- Allows selection of specific audio and subtitle tracks
- Supports chapter creation
- Cross-platform (Windows / macOS / Linux)
- Uses FFmpeg for reliable media handling

## Requirements

- C++11 or later
- CMake 3.10 or later
- FFmpeg libraries (libavformat, libavcodec, libavutil, libavdevice)

## Build Instructions

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
```

### Windows (with MSVC)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## Usage

```bash
bdremux --input /path/to/BDMV --playlist auto --output movie.mkv [options]
```

## Command Line Options

| Option | Short | Description |
|--------|-------|-------------|
| --input | -i | BDMV root directory |
| --playlist | -p | MPLS filename or "auto" for automatic detection |
| --output | -o | Output MKV file path |
| --audio | -a | Audio track indices (comma-separated) or "all" |
| --subtitle | -s | Subtitle track indices (comma-separated) or "all" |
| --chapters | -c | Write chapters to output |
| --verbose | -v | Enable verbose output |
| --help | -h | Show help message |

## Examples

### Basic usage with automatic playlist detection

```bash
bdremux -i /media/bluray/BDMV -o output.mkv
```

### Specify playlist and select specific tracks

```bash
bdremux -i /media/bluray/BDMV -p 00800.mpls -o output.mkv -a 0,2 -s 1
```

### Enable verbose output

```bash
bdremux -i /media/bluray/BDMV -o output.mkv -v
```

## How It Works

1. **Scans BDMV directory**: Validates the Blu-ray structure and locates the PLAYLIST directory
2. **Detects main playlist**: Automatically finds the main movie playlist based on duration and segment count
3. **Parses MPLS file**: Extracts the sequence of M2TS segments and their timing information
4. **Uses FFmpeg concat demuxer**: Combines multiple M2TS files into a single stream
5. **Remuxes to MKV**: Copies all selected streams (video, audio, subtitles) to a new MKV file
6. **Adds chapters**: Creates chapter markers from the playlist information

## Important Notes

- This tool **does not provide any decryption capabilities**
- It only works with already decrypted Blu-rays or homemade/non-encrypted Blu-rays
- You must ensure you have the legal right to use the content you process
- The tool respects the original Blu-ray playlist structure for correct playback order

## License

MIT License

## Disclaimer

This tool is provided for educational and personal use only.

The developers do not condone piracy or any illegal use of this tool.

Users are solely responsible for ensuring their use of this tool complies with applicable laws and regulations.
