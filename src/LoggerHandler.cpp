#include "LoggerHandler.hpp"
#include <ctime>
#include <filesystem>

#ifndef _WIN32
#include <unistd.h>
#endif

// Platform-specific console initialization
void LoggerHandler::initConsole() {
    #ifdef _WIN32
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleHandle == INVALID_HANDLE_VALUE) {
        consoleHandle = nullptr;
    }
    #else
    useColors = isatty(STDOUT_FILENO) != 0;
    #endif
}

// Constructor (shared mutex)
LoggerHandler::LoggerHandler(const std::string& loggerName, std::mutex& consoleMutex)
: loggerName(loggerName),
internalMutex(),
consoleMutex(consoleMutex) {
    initConsole();
}

// Constructor (own mutex)
LoggerHandler::LoggerHandler(const std::string& loggerName)
: loggerName(loggerName),
internalMutex(),
consoleMutex(internalMutex) {
    initConsole();
}

// Destructor
LoggerHandler::~LoggerHandler() {
    disableFileLogging();
}

// File logging – enable
void LoggerHandler::enableFileLogging(const std::string& filePath) {
    std::lock_guard<std::mutex> fileLock(fileMutex);

    if (logFile.is_open()) {
        logFile << "=== Switching to new log file ===\n";
        logFile.close();
    }

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

// File logging – disable
void LoggerHandler::disableFileLogging() {
    std::lock_guard<std::mutex> fileLock(fileMutex);

    if (logFile.is_open()) {
        logFile << "=== Log Ended: " << getCurrentTimestamp() << " ===\n\n";
        logFile.close();

        logToConsole("MESSAGE", "File logging disabled",
                     FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

// Timestamp helper
std::string LoggerHandler::getCurrentTimestamp() {
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

// Set console colour
void LoggerHandler::setConsoleColor(WORD color) {
    #ifdef _WIN32
    if (consoleHandle != nullptr) {
        SetConsoleTextAttribute(consoleHandle, color);
    }
    #else
    if (useColors) {
        std::string ansiCode = "\033[";

        if (color & FOREGROUND_RED) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "91;";
            else ansiCode += "31;";
        }
        if (color & FOREGROUND_GREEN) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "92;";
            else ansiCode += "32;";
        }
        if (color & FOREGROUND_BLUE) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "94;";
            else ansiCode += "34;";
        }

        if ((color & (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)) ==
            (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "97;";
            else ansiCode += "37;";
        }

        if ((color & (FOREGROUND_RED | FOREGROUND_GREEN)) ==
            (FOREGROUND_RED | FOREGROUND_GREEN) &&
            !(color & FOREGROUND_BLUE)) {
            if (color & FOREGROUND_INTENSITY) ansiCode += "93;";
            else ansiCode += "33;";
        }

        if (ansiCode.back() == ';') {
            ansiCode.pop_back();
        }
        ansiCode += "m";
        std::cout << ansiCode;
    }
    #endif
}

// Reset console colour
void LoggerHandler::resetConsoleColor() {
    #ifdef _WIN32
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else
    if (useColors) {
        std::cout << "\033[0m";
    }
    #endif
}

// Format a single log line
std::string LoggerHandler::formatLogLine(const std::string& prefix, const std::string& message) {
    std::string timestamp = getCurrentTimestamp();

    std::ostringstream formattedMessage;
    formattedMessage << "[" << timestamp << "] "
        << "[" << std::left << std::setw(15) << std::setfill('.') << loggerName << "] "
        << "[" << std::left << std::setw(7) << std::setfill('.') << prefix << "] "
        << message;

    return formattedMessage.str();
}

// Log to console
void LoggerHandler::logToConsole(const std::string& prefix, const std::string& message, WORD color) {
    std::lock_guard<std::mutex> consoleLock(consoleMutex);

    std::string formattedMessage = formatLogLine(prefix, message);

    setConsoleColor(color);
    std::cout << formattedMessage << std::endl;
    resetConsoleColor();
}

// Log to file
void LoggerHandler::logToFile(const std::string& prefix, const std::string& message) {
    std::lock_guard<std::mutex> fileLock(fileMutex);

    if (logFile.is_open()) {
        try {
            std::string formattedMessage = formatLogLine(prefix, message);
            logFile << formattedMessage << std::endl;

            if (prefix == "ERROR") {
                logFile.flush();
            }
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> consoleLock(consoleMutex);
            std::cerr << "ERROR: Failed to write to log file: " << e.what() << std::endl;
        }
    }
}

// Public logging methods
void LoggerHandler::logMessage(const std::string& message) {
    logToConsole("MESSAGE", message,
                 FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    logToFile("MESSAGE", message);
}

void LoggerHandler::logSuccess(const std::string& message) {
    logToConsole("SUCCESS", message,
                 FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    logToFile("SUCCESS", message);
}

void LoggerHandler::logWarning(const std::string& message) {
    logToConsole("WARNING", message,
                 FOREGROUND_RED | FOREGROUND_GREEN);
    logToFile("WARNING", message);
}

void LoggerHandler::logError(const std::string& message) {
    logToConsole("ERROR", message,
                 FOREGROUND_RED | FOREGROUND_INTENSITY);
    logToFile("ERROR", message);

    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    std::cout.flush();

    std::lock_guard<std::mutex> fileLock(fileMutex);
    if (logFile.is_open()) {
        logFile.flush();
    }
}