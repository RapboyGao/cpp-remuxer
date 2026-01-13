#include "mpls_parser.h"
#include "logger.h"
#include <fstream>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

// MPLS file constants
const uint32_t MPLS_MAGIC = 0x504D4C53; // "MPLS"
const uint32_t PLAYLIST_MARK_START = 0x4D4C504D; // "MLPM"
const uint32_t PLAY_ITEM_START = 0x5049544D; // "PITM"

// Helper function to read 4 bytes as big-endian uint32_t
uint32_t readBE32(const uint8_t* buffer) {
    return (static_cast<uint32_t>(buffer[0]) << 24) |
           (static_cast<uint32_t>(buffer[1]) << 16) |
           (static_cast<uint32_t>(buffer[2]) << 8) |
           static_cast<uint32_t>(buffer[3]);
}

// Helper function to read 8 bytes as big-endian uint64_t
uint64_t readBE64(const uint8_t* buffer) {
    return (static_cast<uint64_t>(buffer[0]) << 56) |
           (static_cast<uint64_t>(buffer[1]) << 48) |
           (static_cast<uint64_t>(buffer[2]) << 40) |
           (static_cast<uint64_t>(buffer[3]) << 32) |
           (static_cast<uint64_t>(buffer[4]) << 24) |
           (static_cast<uint64_t>(buffer[5]) << 16) |
           (static_cast<uint64_t>(buffer[6]) << 8) |
           static_cast<uint64_t>(buffer[7]);
}

bool parseMPLS(const std::string& mplsPath, const std::string& streamDir, MPLSInfo& mplsInfo) {
    std::ifstream file(mplsPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        Logger::error("Failed to open MPLS file: " + mplsPath);
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        Logger::error("Failed to read MPLS file: " + mplsPath);
        return false;
    }
    
    // Check MPLS magic
    if (size < 16 || readBE32(buffer.data()) != MPLS_MAGIC) {
        Logger::error("Invalid MPLS file: " + mplsPath);
        return false;
    }
    
    mplsInfo.segments.clear();
    mplsInfo.chapterTimes.clear();
    mplsInfo.duration = 0;
    
    uint32_t totalPlayItems = 0;
    uint32_t playItemOffset = 0;
    uint32_t playItemLength = 0;
    
    // Find play item table
    for (uint32_t i = 16; i < size - 8; i += 4) {
        uint32_t chunkType = readBE32(&buffer[i]);
        uint32_t chunkLength = readBE32(&buffer[i + 4]);
        
        if (chunkType == PLAY_ITEM_START) {
            totalPlayItems = readBE32(&buffer[i + 8]);
            playItemOffset = i + 12;
            playItemLength = chunkLength - 8;
            break;
        }
    }
    
    if (totalPlayItems == 0) {
        Logger::error("No play items found in MPLS file: " + mplsPath);
        return false;
    }
    
    // Process each play item
    uint64_t currentChapterTime = 0;
    for (uint32_t i = 0; i < totalPlayItems; ++i) {
        uint32_t itemOffset = playItemOffset + i * playItemLength;
        if (itemOffset + playItemLength > size) {
            Logger::error("Invalid play item offset in MPLS file: " + mplsPath);
            return false;
        }
        
        // Read clip index (16 bytes offset, 2 bytes)
        uint16_t clipIndex = (buffer[itemOffset + 16] << 8) | buffer[itemOffset + 17];
        
        // Read InTime and OutTime (20 and 28 bytes offset, 8 bytes each)
        uint64_t inTime = readBE64(&buffer[itemOffset + 20]);
        uint64_t outTime = readBE64(&buffer[itemOffset + 28]);
        
        // Convert BD time to milliseconds (BD time is in 1/45000 seconds)
        uint64_t startTimeMs = (inTime * 1000) / 45000;
        uint64_t endTimeMs = (outTime * 1000) / 45000;
        uint64_t durationMs = endTimeMs - startTimeMs;
        
        // Generate M2TS filename (00000.m2ts, 00001.m2ts, etc.)
        char m2tsFilename[16];
        sprintf_s(m2tsFilename, sizeof(m2tsFilename), "%05d.m2ts", clipIndex);
        std::string m2tsPath = streamDir + "/" + std::string(m2tsFilename);
        
        // Check if M2TS file exists
        if (!fs::exists(fs::path(m2tsPath))) {
            Logger::error("M2TS file not found: " + m2tsPath);
            return false;
        }
        
        // Add segment
        MPLSSegment segment;
        segment.m2tsPath = m2tsPath;
        segment.startTime = startTimeMs;
        segment.endTime = endTimeMs;
        mplsInfo.segments.push_back(segment);
        
        // Add chapter (one chapter per play item for now)
        mplsInfo.chapterTimes.push_back(currentChapterTime);
        currentChapterTime += durationMs;
        
        // Accumulate total duration
        mplsInfo.duration += durationMs;
    }
    
    // Add final chapter time (end of movie)
    mplsInfo.chapterTimes.push_back(currentChapterTime);
    
    Logger::debug("Parsed MPLS file: " + mplsPath);
    Logger::debug("Segments: " + std::to_string(mplsInfo.segments.size()));
    Logger::debug("Duration: " + std::to_string(mplsInfo.duration / 1000 / 60) + "m " + 
                 std::to_string((mplsInfo.duration / 1000) % 60) + "s");
    Logger::debug("Chapters: " + std::to_string(mplsInfo.chapterTimes.size() - 1));
    
    return true;
}
