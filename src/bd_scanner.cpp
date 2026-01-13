#include "bd_scanner.h"
#include "logger.h"
#include "mpls_parser.h"
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;

bool scanBDStructure(const std::string& bdmvRoot, BDStructure& bdStruct) {
    // Check if BDMV directory exists
    fs::path bdmvPath(bdmvRoot);
    if (!fs::exists(bdmvPath) || !fs::is_directory(bdmvPath)) {
        Logger::error("BDMV directory not found or not a directory: " + bdmvRoot);
        return false;
    }
    
    // Set directory paths
    bdStruct.rootDir = bdmvRoot;
    bdStruct.playlistDir = bdmvRoot + "/PLAYLIST";
    bdStruct.streamDir = bdmvRoot + "/STREAM";
    bdStruct.clipInfoDir = bdmvRoot + "/CLIPINF";
    
    // Check required subdirectories
    if (!fs::exists(fs::path(bdStruct.playlistDir)) || !fs::is_directory(fs::path(bdStruct.playlistDir))) {
        Logger::error("PLAYLIST directory not found: " + bdStruct.playlistDir);
        return false;
    }
    
    if (!fs::exists(fs::path(bdStruct.streamDir)) || !fs::is_directory(fs::path(bdStruct.streamDir))) {
        Logger::error("STREAM directory not found: " + bdStruct.streamDir);
        return false;
    }
    
    if (!fs::exists(fs::path(bdStruct.clipInfoDir)) || !fs::is_directory(fs::path(bdStruct.clipInfoDir))) {
        Logger::error("CLIPINF directory not found: " + bdStruct.clipInfoDir);
        return false;
    }
    
    // Check for index.bdmv
    if (!fs::exists(fs::path(bdmvRoot + "/index.bdmv"))) {
        Logger::error("index.bdmv not found: " + bdmvRoot);
        return false;
    }
    
    // Scan for .mpls files
    bdStruct.playlistFiles.clear();
    for (const auto& entry : fs::directory_iterator(bdStruct.playlistDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mpls") {
            bdStruct.playlistFiles.push_back(entry.path().filename().string());
        }
    }
    
    if (bdStruct.playlistFiles.empty()) {
        Logger::error("No .mpls files found in PLAYLIST directory");
        return false;
    }
    
    Logger::info("Found " + std::to_string(bdStruct.playlistFiles.size()) + " playlist files");
    
    return true;
}

bool findMainPlaylist(const BDStructure& bdStruct, std::string& playlistPath) {
    std::vector<PlaylistInfo> playlistInfos;
    
    // Get information for all playlists
    for (const auto& playlistFile : bdStruct.playlistFiles) {
        std::string fullPath = bdStruct.playlistDir + "/" + playlistFile;
        PlaylistInfo info;
        if (getPlaylistInfo(fullPath, info)) {
            playlistInfos.push_back(info);
        }
    }
    
    if (playlistInfos.empty()) {
        Logger::error("No valid playlist files found");
        return false;
    }
    
    // Find the main playlist: longest duration and segment count > 1
    auto mainPlaylist = std::max_element(playlistInfos.begin(), playlistInfos.end(),
        [](const PlaylistInfo& a, const PlaylistInfo& b) {
            // First priority: duration
            if (a.duration != b.duration) {
                return a.duration < b.duration;
            }
            // Second priority: segment count
            return a.segmentCount < b.segmentCount;
        });
    
    // Check if main playlist has segment count > 1
    if (mainPlaylist->segmentCount <= 1) {
        // If no playlist has segment count > 1, use the longest one anyway
        Logger::warning("No playlist with segment count > 1 found, using longest duration");
    }
    
    playlistPath = mainPlaylist->path;
    Logger::info("Selected main playlist: " + playlistPath);
    Logger::info("Duration: " + std::to_string(mainPlaylist->duration / 1000 / 60) + "m " + 
                 std::to_string((mainPlaylist->duration / 1000) % 60) + "s");
    Logger::info("Segments: " + std::to_string(mainPlaylist->segmentCount));
    
    return true;
}

bool getPlaylistInfo(const std::string& playlistPath, PlaylistInfo& info) {
    MPLSInfo mplsInfo;
    
    // Get stream directory from playlist path
    fs::path path(playlistPath);
    std::string streamDir = path.parent_path().parent_path().string() + "/STREAM";
    
    if (!parseMPLS(playlistPath, streamDir, mplsInfo)) {
        Logger::error("Failed to parse playlist: " + playlistPath);
        return false;
    }
    
    info.path = playlistPath;
    info.duration = mplsInfo.duration;
    info.segmentCount = mplsInfo.segments.size();
    
    return true;
}
