# C++ Logger Library
A simple, cross-platform logging library for C++ with colored console output and file logging support

# Features
- Colored console output (works on Windows, Linux, and macOS)`
- File logging with automatic directory creation
- Thread-safe logging with configurable mutexes
- Timestamped messages with millisecond precision
- Flexible build options (static or shared library)
- Cross-platform (Windows, Linux, macOS)

# Prerequisites
- CMake 3.10 or higher  
- C++11 compatible compiler  

# API Reference
## Constructors
- `Logger(const std::string& loggerName)` - Creates logger with internal mutex  
- `Logger(const std::string& loggerName std::mutex& consoleMutex)` - Uses external mutex  

## Logging Methods
- `logMessage(const std::string& message)` - Regular message (white)  
- `logSuccess(const std::string& message)` - Success message (bright green)  
- `logWarning(const std::string& message)` - Warning message (yellow/orange)  
- `logError(const std::string& message)` - Error message (bright red)  

## File Logging
- `enableFileLogging(const std::string& filePath)` - Start logging to file  
- `disableFileLogging()` - Stop file logging  

# Platform Support

## Windows
- Console colors using Windows API or ANSI escape codes (Windows 10+)  
- Output: Logger.dll (shared) or Logger.lib (static)  

## Linux/macOS
- Console colors using ANSI escape codes  
- Output: libLogger.so (shared) or libLogger.a (static)  


# Build Options
CMake configuration options:  
`BUILD_SHARED_LIBS (default: OFF)` - Build as shared library (.dll/.so)  
`BUILD_STATIC_LIBS (default: ON)` - Build as static library (.lib/.a)  

## Examples for CMake:

For shared library build:  
`cmake .. -DBUILD_AS_SHARED=ON  `

For static (default) library build:  
`cmake .. -DBUILD_AS_SHARED=OFF  `

## Windows Output Files:

### Shared Library (DLL) Build:
- `Logger.dll` - Runtime library (must be distributed with your app)  
- `Logger.lib` - Import library (needed during linking)  

### Static Library Build:
- `Logger.lib` - Static library (contains all code, no DLL needed)  

## Usage:

### For DLL users:
1. Link with `Logger.lib` 
2. Distribute `Logger.dll` with your .exe

### For Static library users:
1. Link with `Logger.lib`
2. No DLL needed

