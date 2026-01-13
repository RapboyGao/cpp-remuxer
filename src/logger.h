#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3
};

class Logger {
public:
    /**
     * Initialize logger
     */
    static void init();
    
    /**
     * Set log level
     * @param level LogLevel to set
     */
    static void setLogLevel(LogLevel level);
    
    /**
     * Get current log level
     * @return Current LogLevel
     */
    static LogLevel getLogLevel();
    
    /**
     * Log error message
     * @param message Error message to log
     */
    static void error(const std::string& message);
    
    /**
     * Log warning message
     * @param message Warning message to log
     */
    static void warning(const std::string& message);
    
    /**
     * Log info message
     * @param message Info message to log
     */
    static void info(const std::string& message);
    
    /**
     * Log debug message
     * @param message Debug message to log
     */
    static void debug(const std::string& message);
    
private:
    static LogLevel currentLevel;
    static const char* levelToString(LogLevel level);
};

#endif // LOGGER_H
