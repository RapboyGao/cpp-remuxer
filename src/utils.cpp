#include "utils.h"
#include <filesystem>
#include <sstream>
#include <ctime>
#include <random>
#include <iomanip>

namespace fs = std::filesystem;

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool fileExists(const std::string& filePath) {
    return fs::exists(fs::path(filePath));
}

bool createDirectory(const std::string& dirPath) {
    return fs::create_directories(fs::path(dirPath));
}

std::string generateTempFilePath(const std::string& prefix, const std::string& suffix) {
    // Generate random number for uniqueness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    int randomNum = dis(gen);
    
    // Get current time for additional uniqueness
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    #ifdef _WIN32
    localtime_s(&localTime, &now);
    #else
    localtime_r(&now, &localTime);
    #endif
    
    // Format temp file path
    std::ostringstream oss;
    oss << prefix << "_" 
        << (localTime.tm_year + 1900) 
        << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1)
        << std::setw(2) << std::setfill('0') << localTime.tm_mday
        << "_" 
        << std::setw(2) << std::setfill('0') << localTime.tm_hour
        << std::setw(2) << std::setfill('0') << localTime.tm_min
        << std::setw(2) << std::setfill('0') << localTime.tm_sec
        << "_" << randomNum << suffix;
    
    return oss.str();
}

std::string msToTimeString(uint64_t ms) {
    uint64_t hours = ms / 3600000;
    ms %= 3600000;
    uint64_t minutes = ms / 60000;
    ms %= 60000;
    uint64_t seconds = ms / 1000;
    ms %= 1000;
    
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":" 
        << std::setw(2) << std::setfill('0') << minutes << ":" 
        << std::setw(2) << std::setfill('0') << seconds << "." 
        << std::setw(3) << std::setfill('0') << ms;
    
    return oss.str();
}
