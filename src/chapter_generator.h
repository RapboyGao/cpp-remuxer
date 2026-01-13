#ifndef CHAPTER_GENERATOR_H
#define CHAPTER_GENERATOR_H

#include <string>
#include <vector>
#include <libavformat/avformat.h>
#include "mpls_parser.h"

/**
 * Add chapters to output format context
 * @param outFmt Output AVFormatContext
 * @param mplsInfo MPLSInfo containing chapter times
 * @return True if chapters added successfully, false otherwise
 */
bool addChaptersToOutput(AVFormatContext* outFmt, const MPLSInfo& mplsInfo);

/**
 * Generate chapter metadata string in Matroska format
 * @param mplsInfo MPLSInfo containing chapter times
 * @return Chapter metadata string
 */
std::string generateChapterMetadata(const MPLSInfo& mplsInfo);

#endif // CHAPTER_GENERATOR_H
