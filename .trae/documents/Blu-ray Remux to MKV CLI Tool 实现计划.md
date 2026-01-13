# Blu-ray Remux to MKV CLI Tool 实现计划

## 1. 项目结构搭建
- 创建CMakeLists.txt文件，配置FFmpeg依赖
- 建立src目录结构，包含核心模块
- 创建主入口文件main.cpp

## 2. 核心模块设计

### 2.1 CLI参数解析
- 使用getopt或类似库处理命令行参数
- 支持--input, --playlist, --output, --audio, --subtitle, --chapters, --verbose参数
- 实现参数验证和帮助信息

### 2.2 Blu-ray扫描模块
- 实现BDMV目录结构验证
- 遍历PLAYLIST目录下的所有.mpls文件
- 实现自动主节目检测逻辑（最长时长，segment数>1）

### 2.3 MPLS解析器
- 解析.mpls文件格式
- 提取播放列表的segment信息和时长
- 生成M2TS片段路径列表

### 2.4 FFmpeg Remux核心
- 实现内存concat文件生成
- 使用FFmpeg库打开多个M2TS文件
- 创建MKV输出上下文
- 实现stream copy逻辑
- 处理音视频字幕流映射

### 2.5 章节处理
- 从MPLS的PlayItem Time In/Out生成章节信息
- 使用AVChapter结构写入MKV

### 2.6 错误处理与日志
- 实现不同级别的日志输出
- 明确区分错误类型并返回适当的错误码
- 确保所有FFmpeg对象正确释放

## 3. 技术实现细节

### 3.1 依赖管理
- 使用CMake的pkg_check_modules查找FFmpeg库
- 支持跨平台编译（Windows/macOS/Linux）

### 3.2 代码结构
```
src/
├── main.cpp              # 主入口
├── cli_parser.h/cpp      # CLI参数解析
├── bd_scanner.h/cpp      # Blu-ray扫描
├── mpls_parser.h/cpp     # MPLS解析
├── remuxer.h/cpp         # FFmpeg remux核心
├── chapter_generator.h/cpp # 章节生成
├── logger.h/cpp          # 日志系统
└── utils.h/cpp           # 工具函数
```

### 3.3 关键算法
- MPLS解析算法：处理二进制MPLS文件格式
- 主节目检测算法：基于时长和segment数量选择
- Stream映射算法：确保正确的音视频字幕流复制

## 4. 测试与验证
- 实现基本的单元测试
- 测试不同类型的Blu-ray结构
- 验证remux后的MKV文件完整性

## 5. 编译与部署
- 提供详细的编译说明
- 支持静态编译选项（可选）
- 生成跨平台可执行文件

## 6. 合规性
- 在代码中添加明确的免责声明
- 确保不包含任何解密功能
- 遵守FFmpeg的许可证要求

## 7. 开发时间表
- 项目结构和CMake配置：1天
- CLI参数解析：1天
- Blu-ray扫描和MPLS解析：2天
- FFmpeg Remux核心实现：2天
- 章节处理和错误处理：1天
- 测试和优化：1天

这个计划将确保我们开发出一个功能完整、稳定可靠的Blu-ray Remux到MKV的CLI工具，符合所有技术要求和合规性标准。