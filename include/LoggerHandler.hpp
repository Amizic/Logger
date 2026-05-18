#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <sstream>

// DLL export / import (Windows only)
#ifdef _WIN32
    #if defined(LOGGER_DYNAMIC)
        #if defined(LOGGER_BUILD)
            #define LOGGER_API __declspec(dllexport)
        #else
            #define LOGGER_API __declspec(dllimport)
        #endif
    #else
        #define LOGGER_API
    #endif
#else
    #define LOGGER_API
#endif

#ifdef _WIN32
    #include <windows.h>
#else
    // Linux/macOS console colours via ANSI escape codes
    #define FOREGROUND_RED          0x0001
    #define FOREGROUND_GREEN        0x0002
    #define FOREGROUND_BLUE         0x0004
    #define FOREGROUND_INTENSITY    0x0008
    typedef unsigned short WORD;
#endif

class LOGGER_API LoggerHandler {
public:
    // Constructors & Destructor
    LoggerHandler(const std::string& loggerName, std::mutex& consoleMutex);
    LoggerHandler(const std::string& loggerName);
    ~LoggerHandler();

    // File logging
    void enableFileLogging(const std::string& filePath);
    void disableFileLogging();

    // Logging methods
    void logMessage(const std::string& message);
    void logSuccess(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);

private:
    void logToConsole(const std::string& prefix, const std::string& message, WORD color);
    void logToFile(const std::string& prefix, const std::string& message);
    void setConsoleColor(WORD color);
    void resetConsoleColor();
    std::string getCurrentTimestamp();
    std::string formatLogLine(const std::string& prefix, const std::string& message);
    void initConsole();

    std::string loggerName;
    std::mutex internalMutex;
    std::mutex& consoleMutex;

#ifdef _WIN32
    HANDLE consoleHandle;
#else
    bool useColors;
#endif

    std::ofstream logFile;
    std::mutex fileMutex;
};