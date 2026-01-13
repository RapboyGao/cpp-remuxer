#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>

/**
 * Create temporary test directory
 * @return Path to temporary directory
 */
std::string createTempTestDir();

/**
 * Create test MPLS file
 * @param dirPath Directory to create MPLS file in
 * @param filename MPLS filename
 * @param content MPLS file content
 * @return True if file created successfully, false otherwise
 */
bool createTestMPLSFile(const std::string& dirPath, const std::string& filename, const std::vector<uint8_t>& content);

/**
 * Cleanup temporary test directory
 * @param dirPath Path to temporary directory to cleanup
 */
void cleanupTempTestDir(const std::string& dirPath);

#endif // TEST_UTILS_H
