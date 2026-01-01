#ifdef __linux__
#include "../include/logger/Logger.hpp"
#else
#include "include/Logger/Logger.hpp"
#endif
#include <ctime>
#include <filesystem>
#include <unistd.h>
// Platform-specific console initialization
void Logger::initConsole() {
#ifdef _WIN32
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleHandle == INVALID_HANDLE_VALUE) {
        consoleHandle = nullptr;
    }
#else
    // Check if output is a terminal (supports colors)
    useColors = isatty(STDOUT_FILENO) != 0;
#endif
}

// Constructor
Logger::Logger(const std::string& loggerName, std::mutex& consoleMutex)
    : loggerName(loggerName), 
      internalMutex(),
      consoleMutex(consoleMutex) {
    initConsole();
}

// Uses internally owned mutex
Logger::Logger(const std::string& loggerName)
    : loggerName(loggerName),
      internalMutex(),                  // construct internal mutex
      consoleMutex(internalMutex) {     // bind reference to it
    initConsole();
}

// Destructor
Logger::~Logger() {
    disableFileLogging();
}

// File logging methods
void Logger::enableFileLogging(const std::string& filePath) {
    std::lock_guard<std::mutex> fileLock(fileMutex);
    
    if (logFile.is_open()) {
        logFile << "=== Switching to new log file ===\n";
        logFile.close();
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filePath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    logFile.open(filePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << "=== Log Started: " << getCurrentTimestamp() << " ===\n";
        logFile << "Logger: " << loggerName << "\n";
        logFile << "===================================\n";
        logFile.flush();
        
        logToConsole("MESSAGE", "File logging enabled: " + filePath, 
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else {
        logToConsole("ERROR", "Failed to open log file: " + filePath,
                    FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
}

void Logger::disableFileLogging() {
    std::lock_guard<std::mutex> fileLock(fileMutex);
    
    if (logFile.is_open()) {
        logFile << "=== Log Ended: " << getCurrentTimestamp() << " ===\n\n";
        logFile.close();
        
        logToConsole("MESSAGE", "File logging disabled", 
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

// Private helpers
std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);
    
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;
    
    std::tm timeBuffer;
    
#ifdef _WIN32
    localtime_s(&timeBuffer, &inTimeT);
#else
    localtime_r(&inTimeT, &timeBuffer);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&timeBuffer, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
}

void Logger::setConsoleColor(WORD color) {
#ifdef _WIN32
    if (consoleHandle != nullptr) {
        SetConsoleTextAttribute(consoleHandle, color);
    }
#else
    if (useColors) {
        std::string ansiCode = "\033[";
        
        // Map Windows color codes to ANSI codes
        if (color & FOREGROUND_RED) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "91;";  // Bright red
            else ansiCode += "31;";  // Normal red
        }
        if (color & FOREGROUND_GREEN) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "92;";  // Bright green
            else ansiCode += "32;";  // Normal green
        }
        if (color & FOREGROUND_BLUE) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "94;";  // Bright blue
            else ansiCode += "34;";  // Normal blue
        }
        
        // Handle white/gray (R+G+B)
        if ((color & (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)) == 
            (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "97;";  // Bright white
            else ansiCode += "37;";  // Normal white/gray
        }
        
        // Handle yellow (R+G)
        if ((color & (FOREGROUND_RED | FOREGROUND_GREEN)) == 
            (FOREGROUND_RED | FOREGROUND_GREEN) && 
            !(color & FOREGROUND_BLUE)) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "93;";  // Bright yellow
            else ansiCode += "33;";  // Normal yellow
        }
        
        if (ansiCode.back() == ';') {
            ansiCode.pop_back();  // Remove trailing semicolon
        }
        ansiCode += "m";
        
        std::cout << ansiCode;
    }
#endif
}

void Logger::resetConsoleColor() {
#ifdef _WIN32
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    if (useColors) {
        std::cout << "\033[0m";  // Reset colors
    }
#endif
}

std::string Logger::formatLogLine(const std::string& prefix, const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    
    std::ostringstream formattedMessage;
    formattedMessage << "[" << timestamp << "] "
                     << "[" << std::left << std::setw(15) << std::setfill('.') << loggerName << "] "
                     << "[" << std::left << std::setw(7) << std::setfill('.') << prefix << "] "
                     << message;
    
    return formattedMessage.str();
}

// Console logging
void Logger::logToConsole(const std::string& prefix, const std::string& message, WORD color) {
    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    
    std::string formattedMessage = formatLogLine(prefix, message);
    
    setConsoleColor(color);
    std::cout << formattedMessage << std::endl;
    resetConsoleColor();
}

// File logging
void Logger::logToFile(const std::string& prefix, const std::string& message) {
    std::lock_guard<std::mutex> fileLock(fileMutex);
    
    if (logFile.is_open()) {
        try {
            std::string formattedMessage = formatLogLine(prefix, message);
            logFile << formattedMessage << std::endl;
            
            if (prefix == "ERROR") {
                logFile.flush();
            }
        } catch (const std::exception& e) {
            // Can't use logToConsole here (would deadlock), so use basic output
            std::lock_guard<std::mutex> consoleLock(consoleMutex);
            std::cerr << "ERROR: Failed to write to log file: " << e.what() << std::endl;
        }
    }
}

// Public logging methods
void Logger::logMessage(const std::string& message) {
    logToConsole("MESSAGE", message, 
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    logToFile("MESSAGE", message);
}

void Logger::logSuccess(const std::string& message) {
    logToConsole("SUCCESS", message, 
                FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    logToFile("SUCCESS", message);
}

void Logger::logWarning(const std::string& message) {
    logToConsole("WARNING", message, 
                FOREGROUND_RED | FOREGROUND_GREEN);
    logToFile("WARNING", message);
}

void Logger::logError(const std::string& message) {
    logToConsole("ERROR", message, 
                FOREGROUND_RED | FOREGROUND_INTENSITY);
    logToFile("ERROR", message);
    
    // Force flush for errors
    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    std::cout.flush();
    
    std::lock_guard<std::mutex> fileLock(fileMutex);
    if (logFile.is_open()) {
        logFile.flush();
    }

}
