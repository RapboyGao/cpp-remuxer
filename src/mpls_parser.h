#ifndef MPLS_PARSER_H
#define MPLS_PARSER_H

#include <string>
#include <vector>

struct MPLSSegment {
    std::string m2tsPath;      // Path to M2TS file
    uint64_t startTime;        // Start time in milliseconds
    uint64_t endTime;          // End time in milliseconds
};

struct MPLSInfo {
    uint64_t duration;         // Total duration in milliseconds
    std::vector<MPLSSegment> segments;  // List of segments
    std::vector<uint64_t> chapterTimes; // Chapter times in milliseconds
};

/**
 * Parse MPLS file
 * @param mplsPath Path to .mpls file
 * @param streamDir Path to STREAM directory
 * @param mplsInfo Reference to MPLSInfo structure to fill
 * @return True if parsing succeeded, false otherwise
 */
bool parseMPLS(const std::string& mplsPath, const std::string& streamDir, MPLSInfo& mplsInfo);

#endif // MPLS_PARSER_H
