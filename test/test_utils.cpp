#include "test_utils.h"
#include <filesystem>
#include <fstream>
#include <random>

namespace fs = std::filesystem;

std::string createTempTestDir() {
    // Generate random directory name
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    int randomNum = dis(gen);
    
    std::string dirPath = "test_temp_" + std::to_string(randomNum);
    
    // Create directory
    if (!fs::create_directories(fs::path(dirPath))) {
        return "";
    }
    
    return dirPath;
}

bool createTestMPLSFile(const std::string& dirPath, const std::string& filename, const std::vector<uint8_t>& content) {
    std::string filePath = dirPath + "/" + filename;
    std::ofstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
    file.close();
    
    return true;
}

void cleanupTempTestDir(const std::string& dirPath) {
    if (!dirPath.empty()) {
        fs::remove_all(fs::path(dirPath));
    }
}
