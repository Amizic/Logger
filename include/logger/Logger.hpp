#pragma once
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <sstream>

#ifdef _WIN32

    #ifdef LOGGER_EXPORTS
        #define LOGGER_API __declspec(dllexport)
    #elif defined(LOGGER_STATIC_DEFINE)
        #define LOGGER_API
    #else
        #define LOGGER_API __declspec(dllimport)
    #endif
    
    #include <windows.h>
    
#else
    #define LOGGER_API
    // Linux/macOS console colors using ANSI escape codes
    #define FOREGROUND_RED          0x0001
    #define FOREGROUND_GREEN        0x0002  
    #define FOREGROUND_BLUE         0x0004
    #define FOREGROUND_INTENSITY    0x0008
    typedef unsigned short WORD;
#endif

class Logger {
public:
    // Constructor & Destructor
    Logger(const std::string& loggerName, std::mutex& consoleMutex);
    Logger(const std::string& loggerName);
    ~Logger();
    
    // File logging methods
    void enableFileLogging(const std::string& filePath);
    void disableFileLogging();
    
    // Simple logging methods
    void logMessage(const std::string& message);
    void logSuccess(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    
private:
    // Helper methods
    void logToConsole(const std::string& prefix, const std::string& message, WORD color);
    void logToFile(const std::string& prefix, const std::string& message);
    void setConsoleColor(WORD color);
    void resetConsoleColor();
    std::string getCurrentTimestamp();
    std::string formatLogLine(const std::string& prefix, const std::string& message);
    
    // Platform-specific console initialization
    void initConsole();
    
    // Member variables
    std::string loggerName;
    std::mutex internalMutex;
    std::mutex& consoleMutex;
    
#ifdef _WIN32
    HANDLE consoleHandle;
#else
    bool useColors;
#endif
    
    // File logging
    std::ofstream logFile;
    std::mutex fileMutex;
};