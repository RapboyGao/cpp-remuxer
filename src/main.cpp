#include <iostream>
#include <string>
#include "cli_parser.h"
#include "bd_scanner.h"
#include "mpls_parser.h"
#include "remuxer.h"
#include "logger.h"

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
    
    // Remux to MKV
    if (!remuxToMKV(mplsInfo, args, bdStruct.streamDir)) {
        Logger::error("Failed to remux to MKV");
        return 1;
    }
    
    Logger::info("Remux completed successfully!");
    
    return 0;
}
