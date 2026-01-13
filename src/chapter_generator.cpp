#include "chapter_generator.h"
#include "logger.h"
#include "utils.h"
#include <libavutil/avutil.h>
#include <libavutil/time.h>

bool addChaptersToOutput(AVFormatContext* outFmt, const MPLSInfo& mplsInfo) {
    if (!outFmt) {
        Logger::error("Invalid output format context for adding chapters");
        return false;
    }
    
    if (mplsInfo.chapterTimes.size() < 2) {
        Logger::warning("Not enough chapter times to add chapters");
        return true; // Not an error, just no chapters to add
    }
    
    // Get time base from first stream (assuming all streams have the same time base)
    AVRational timeBase = {1, 1000}; // Default to milliseconds
    if (outFmt->nb_streams > 0 && outFmt->streams[0]) {
        timeBase = outFmt->streams[0]->time_base;
    }
    
    Logger::info("Adding " + std::to_string(mplsInfo.chapterTimes.size() - 1) + " chapters to output");
    
    // Add each chapter
    for (size_t i = 0; i < mplsInfo.chapterTimes.size() - 1; ++i) {
        uint64_t startTimeMs = mplsInfo.chapterTimes[i];
        uint64_t endTimeMs = mplsInfo.chapterTimes[i + 1];
        
        // Create chapter
        AVChapter* chapter = av_chapter_alloc();
        if (!chapter) {
            Logger::error("Failed to allocate chapter");
            return false;
        }
        
        // Set chapter ID and time base
        chapter->id = i + 1;
        chapter->time_base = timeBase;
        
        // Convert milliseconds to AV_TIME_BASE units
        chapter->start = av_rescale_q(startTimeMs * 1000, {1, AV_TIME_BASE}, timeBase);
        chapter->end = av_rescale_q(endTimeMs * 1000, {1, AV_TIME_BASE}, timeBase);
        
        // Set chapter metadata
        char chapterKey[32];
        sprintf(chapterKey, "title%02d", static_cast<int>(i + 1));
        char chapterTitle[64];
        sprintf(chapterTitle, "Chapter %d", static_cast<int>(i + 1));
        
        AVDictionaryEntry* tag = av_dict_get(chapter->metadata, "title", nullptr, 0);
        if (tag) {
            av_dict_set(&chapter->metadata, "title", chapterTitle, 0);
        } else {
            av_dict_set(&chapter->metadata, "title", chapterTitle, AV_DICT_IGNORE_SUFFIX);
        }
        
        // Add chapter to format context
        if (avformat_new_chapter(outFmt, i, timeBase, chapter->start, chapter->end, &chapter->metadata) < 0) {
            Logger::error("Failed to add chapter " + std::to_string(i + 1));
            av_chapter_free(&chapter);
            return false;
        }
        
        Logger::debug("Added chapter " + std::to_string(i + 1) + ": " + 
                     msToTimeString(startTimeMs) + " - " + msToTimeString(endTimeMs));
        
        av_chapter_free(&chapter);
    }
    
    return true;
}

std::string generateChapterMetadata(const MPLSInfo& mplsInfo) {
    std::ostringstream oss;
    
    for (size_t i = 0; i < mplsInfo.chapterTimes.size() - 1; ++i) {
        uint64_t startTimeMs = mplsInfo.chapterTimes[i];
        uint64_t endTimeMs = mplsInfo.chapterTimes[i + 1];
        
        // Convert to Matroska chapter format (milliseconds)
        oss << "CHAPTER" << (i + 1) << "=" << startTimeMs << "\n";
        oss << "CHAPTER" << (i + 1) << "NAME=Chapter " << (i + 1) << "\n";
    }
    
    return oss.str();
}
