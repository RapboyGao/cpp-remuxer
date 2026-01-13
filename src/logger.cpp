#include "logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>

LogLevel Logger::currentLevel = LogLevel::INFO;

void Logger::init() {
    // Initialize logger if needed
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

LogLevel Logger::getLogLevel() {
    return currentLevel;
}

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

void Logger::error(const std::string& message) {
    if (currentLevel >= LogLevel::ERROR) {
        std::cerr << "[" << levelToString(LogLevel::ERROR) << "] " << message << std::endl;
    }
}

void Logger::warning(const std::string& message) {
    if (currentLevel >= LogLevel::WARNING) {
        std::cout << "[" << levelToString(LogLevel::WARNING) << "] " << message << std::endl;
    }
}

void Logger::info(const std::string& message) {
    if (currentLevel >= LogLevel::INFO) {
        std::cout << "[" << levelToString(LogLevel::INFO) << "] " << message << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    if (currentLevel >= LogLevel::DEBUG) {
        std::cout << "[" << levelToString(LogLevel::DEBUG) << "] " << message << std::endl;
    }
}
