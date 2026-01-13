#include "remuxer.h"
#include "logger.h"
#include "chapter_generator.h"
#include "utils.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <fstream>
#include <vector>

std::string generateConcatFileContent(const std::vector<MPLSSegment>& segments) {
    std::ostringstream oss;
    
    for (const auto& segment : segments) {
        oss << "file '" << segment.m2tsPath << "'" << std::endl;
    }
    
    return oss.str();
}

bool remuxToMKV(const MPLSInfo& mplsInfo, const CLIArgs& args, const std::string& streamDir) {
    if (mplsInfo.segments.empty()) {
        Logger::error("No segments to remux");
        return false;
    }
    
    // Register all formats and codecs
    av_register_all();
    avformat_network_init();
    
    AVFormatContext* inFmt = nullptr;
    AVFormatContext* outFmt = nullptr;
    AVPacket pkt;
    int ret;
    
    // Generate concat file content
    std::string concatContent = generateConcatFileContent(mplsInfo.segments);
    Logger::debug("Generated concat content:\n" + concatContent);
    
    // Create temporary concat file
    std::string concatFilePath = generateTempFilePath("bdremux", ".txt");
    std::ofstream concatFile(concatFilePath);
    if (!concatFile.is_open()) {
        Logger::error("Failed to create concat file: " + concatFilePath);
        return false;
    }
    
    concatFile << concatContent;
    concatFile.close();
    
    // Open input file using concat demuxer
    Logger::info("Opening input files...");
    if ((ret = avformat_open_input(&inFmt, concatFilePath.c_str(), nullptr, nullptr)) < 0) {
        Logger::error("Failed to open input: " + std::string(av_err2str(ret)));
        std::remove(concatFilePath.c_str());
        return false;
    }
    
    // Get stream information
    if ((ret = avformat_find_stream_info(inFmt, nullptr)) < 0) {
        Logger::error("Failed to retrieve stream information: " + std::string(av_err2str(ret)));
        avformat_close_input(&inFmt);
        std::remove(concatFilePath.c_str());
        return false;
    }
    
    // Print input stream information
    av_dump_format(inFmt, 0, concatFilePath.c_str(), 0);
    
    // Create output format context
    Logger::info("Creating output file: " + args.output);
    if ((ret = avformat_alloc_output_context2(&outFmt, nullptr, "matroska", args.output.c_str())) < 0) {
        Logger::error("Failed to allocate output format context: " + std::string(av_err2str(ret)));
        avformat_close_input(&inFmt);
        std::remove(concatFilePath.c_str());
        return false;
    }
    
    // Create output streams
    std::vector<int> streamMap(inFmt->nb_streams, -1);
    int outStreamIndex = 0;
    
    for (int i = 0; i < inFmt->nb_streams; i++) {
        AVStream* inStream = inFmt->streams[i];
        AVCodec* codec = avcodec_find_decoder(inStream->codecpar->codec_id);
        
        if (!codec) {
            Logger::warning("Codec not found for stream " + std::to_string(i) + ", skipping");
            continue;
        }
        
        // Create output stream
        AVStream* outStream = avformat_new_stream(outFmt, codec);
        if (!outStream) {
            Logger::error("Failed to allocate output stream");
            avformat_close_input(&inFmt);
            avformat_free_context(outFmt);
            std::remove(concatFilePath.c_str());
            return false;
        }
        
        // Copy codec parameters
        if ((ret = avcodec_parameters_copy(outStream->codecpar, inStream->codecpar)) < 0) {
            Logger::error("Failed to copy codec parameters: " + std::string(av_err2str(ret)));
            avformat_close_input(&inFmt);
            avformat_free_context(outFmt);
            std::remove(concatFilePath.c_str());
            return false;
        }
        
        outStream->codecpar->codec_tag = 0;
        outStream->time_base = inStream->time_base;
        
        streamMap[i] = outStreamIndex++;
        
        Logger::debug("Mapped input stream " + std::to_string(i) + " to output stream " + std::to_string(outStreamIndex - 1));
    }
    
    // Print output stream information
    av_dump_format(outFmt, 0, args.output.c_str(), 1);
    
    // Open output file
    if (!(outFmt->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&outFmt->pb, args.output.c_str(), AVIO_FLAG_WRITE)) < 0) {
            Logger::error("Failed to open output file: " + std::string(av_err2str(ret)));
            avformat_close_input(&inFmt);
            avformat_free_context(outFmt);
            std::remove(concatFilePath.c_str());
            return false;
        }
    }
    
    // Write header
    if ((ret = avformat_write_header(outFmt, nullptr)) < 0) {
        Logger::error("Failed to write output header: " + std::string(av_err2str(ret)));
        avformat_close_input(&inFmt);
        if (!(outFmt->oformat->flags & AVFMT_NOFILE)) {
            avio_close(outFmt->pb);
        }
        avformat_free_context(outFmt);
        std::remove(concatFilePath.c_str());
        return false;
    }
    
    // Add chapters if requested
    if (args.chapters) {
        if (!addChaptersToOutput(outFmt, mplsInfo)) {
            Logger::warning("Failed to add chapters, continuing without chapters");
        }
    }
    
    // Remux loop
    Logger::info("Starting remux process...");
    int frameCount = 0;
    
    while (av_read_frame(inFmt, &pkt) >= 0) {
        AVStream* inStream = inFmt->streams[pkt.stream_index];
        
        // Check if stream is mapped
        if (streamMap[pkt.stream_index] < 0) {
            av_packet_unref(&pkt);
            continue;
        }
        
        // Map stream index
        AVStream* outStream = outFmt->streams[streamMap[pkt.stream_index]];
        pkt.stream_index = streamMap[pkt.stream_index];
        
        // Convert packet timestamps to output time base
        av_packet_rescale_ts(&pkt, inStream->time_base, outStream->time_base);
        
        pkt.pos = -1;
        
        // Write packet
        if ((ret = av_interleaved_write_frame(outFmt, &pkt)) < 0) {
            Logger::error("Failed to write packet: " + std::string(av_err2str(ret)));
            av_packet_unref(&pkt);
            break;
        }
        
        av_packet_unref(&pkt);
        
        // Print progress every 1000 frames
        if (frameCount % 1000 == 0) {
            Logger::info("Processed " + std::to_string(frameCount) + " frames");
        }
        frameCount++;
    }
    
    // Write trailer
    av_write_trailer(outFmt);
    
    // Cleanup
    avformat_close_input(&inFmt);
    
    if (!(outFmt->oformat->flags & AVFMT_NOFILE)) {
        avio_close(outFmt->pb);
    }
    avformat_free_context(outFmt);
    
    // Remove temporary concat file
    std::remove(concatFilePath.c_str());
    
    Logger::info("Remux completed, processed " + std::to_string(frameCount) + " frames");
    
    return true;
}
