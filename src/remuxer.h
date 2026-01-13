#ifndef REMUXER_H
#define REMUXER_H

#include <string>
#include "mpls_parser.h"
#include "cli_parser.h"

/**
 * Remux Blu-ray playlist to MKV file
 * @param mplsInfo MPLSInfo containing playlist segments
 * @param args CLI arguments
 * @param streamDir Path to STREAM directory
 * @return True if remux succeeded, false otherwise
 */
bool remuxToMKV(const MPLSInfo& mplsInfo, const CLIArgs& args, const std::string& streamDir);

/**
 * Generate concat file content for FFmpeg concat demuxer
 * @param segments List of MPLSSegment
 * @return Concat file content as string
 */
std::string generateConcatFileContent(const std::vector<MPLSSegment>& segments);

#endif // REMUXER_H
