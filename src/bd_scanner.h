#ifndef BD_SCANNER_H
#define BD_SCANNER_H

#include <string>
#include <vector>

struct BDStructure {
    std::string rootDir;       // BDMV root directory
    std::string playlistDir;   // PLAYLIST directory path
    std::string streamDir;     // STREAM directory path
    std::string clipInfoDir;   // CLIPINF directory path
    std::vector<std::string> playlistFiles; // List of .mpls files
};

struct PlaylistInfo {
    std::string path;          // Path to the playlist file
    uint64_t duration;         // Duration in milliseconds
    size_t segmentCount;       // Number of segments
};

/**
 * Scan Blu-ray structure
 * @param bdmvRoot BDMV root directory
 * @param bdStruct Reference to BDStructure to fill
 * @return True if scan succeeded, false otherwise
 */
bool scanBDStructure(const std::string& bdmvRoot, BDStructure& bdStruct);

/**
 * Find main playlist automatically
 * @param bdStruct BDStructure containing playlist files
 * @param playlistPath Reference to string to fill with playlist path
 * @return True if main playlist found, false otherwise
 */
bool findMainPlaylist(const BDStructure& bdStruct, std::string& playlistPath);

/**
 * Get playlist information
 * @param playlistPath Path to .mpls file
 * @param info Reference to PlaylistInfo to fill
 * @return True if info retrieved, false otherwise
 */
bool getPlaylistInfo(const std::string& playlistPath, PlaylistInfo& info);

#endif // BD_SCANNER_H
