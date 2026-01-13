#include <iostream>
#include <string>
#include "cli_parser.h"
#include "bd_scanner.h"
#include "mpls_parser.h"
#include "logger.h"

#ifndef BUILD_MINIMAL
#include "remuxer.h"
#endif

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::init();
    
    // Parse command line arguments
    CLIArgs args;
    if (!parseCLIArgs(argc, argv, args)) {
        Logger::error("Failed to parse command line arguments");
        return 1;
    }
    
    // Set log level based on verbose flag
    if (args.verbose) {
        Logger::setLogLevel(LogLevel::DEBUG);
    }
    
    Logger::info("Starting bdremux...");
    Logger::debug("Input: " + args.input);
    Logger::debug("Output: " + args.output);
    
    // Check if running in minimal mode
    #ifdef BUILD_MINIMAL
    Logger::warning("Running in minimal mode (no FFmpeg support)");
    Logger::warning("This build only supports Blu-ray scanning and MPLS parsing");
    Logger::warning("To enable remuxing, rebuild with FFmpeg libraries");
    #endif
    
    // Scan Blu-ray structure
    BDStructure bdStruct;
    if (!scanBDStructure(args.input, bdStruct)) {
        Logger::error("Failed to scan Blu-ray structure");
        return 1;
    }
    
    // Find playlist
    std::string playlistPath;
    if (args.playlist == "auto") {
        if (!findMainPlaylist(bdStruct, playlistPath)) {
            Logger::error("Failed to find main playlist");
            return 1;
        }
    } else {
        playlistPath = args.input + "/PLAYLIST/" + args.playlist;
    }
    
    Logger::info("Using playlist: " + playlistPath);
    
    // Parse MPLS file
    MPLSInfo mplsInfo;
    if (!parseMPLS(playlistPath, bdStruct.streamDir, mplsInfo)) {
        Logger::error("Failed to parse MPLS file");
        return 1;
    }
    
    // Remux to MKV (only if FFmpeg is available)
    #ifndef BUILD_MINIMAL
    if (!remuxToMKV(mplsInfo, args, bdStruct.streamDir)) {
        Logger::error("Failed to remux to MKV");
        return 1;
    }
    
    Logger::info("Remux completed successfully!");
    #else
    Logger::info("MPLS parsing completed successfully!");
    Logger::info("Found " + std::to_string(mplsInfo.segments.size()) + " segments");
    Logger::info("Duration: " + std::to_string(mplsInfo.duration / 1000 / 60) + "m " + 
                 std::to_string((mplsInfo.duration / 1000) % 60) + "s");
    Logger::info("Chapters: " + std::to_string(mplsInfo.chapterTimes.size() - 1));
    
    // Print segment information
    for (size_t i = 0; i < mplsInfo.segments.size(); i++) {
        const auto& segment = mplsInfo.segments[i];
        Logger::debug("Segment " + std::to_string(i + 1) + ": " + segment.m2tsPath);
    }
    #endif
    
    return 0;
}
