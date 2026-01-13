#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

/**
 * Split string by delimiter
 * @param str String to split
 * @param delimiter Delimiter character
 * @return Vector of split strings
 */
std::vector<std::string> splitString(const std::string& str, char delimiter);

/**
 * Check if file exists
 * @param filePath Path to file
 * @return True if file exists, false otherwise
 */
bool fileExists(const std::string& filePath);

/**
 * Create directory recursively
 * @param dirPath Path to directory
 * @return True if directory created or exists, false otherwise
 */
bool createDirectory(const std::string& dirPath);

/**
 * Generate temporary file path
 * @param prefix Prefix for temporary file name
 * @param suffix Suffix for temporary file name
 * @return Temporary file path
 */
std::string generateTempFilePath(const std::string& prefix = "bdremux", const std::string& suffix = ".txt");

/**
 * Convert milliseconds to time string (HH:MM:SS.mmm)
 * @param ms Milliseconds
 * @return Time string
 */
std::string msToTimeString(uint64_t ms);

#endif // UTILS_H
