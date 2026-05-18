# C++ Logger Library
A simple, cross-platform logging library for C++ with coloured console output and file logging support.

## Features
- Coloured console output (Windows, Linux, macOS)
- File logging with automatic directory creation
- Thread‑safe logging with configurable mutexes
- Timestamped messages with millisecond precision
- Flexible build options (static or shared library)
- Cross‑platform (Windows, Linux, macOS)

## Prerequisites
- C++17 compatible compiler (GCC ≥ 8, Clang ≥ 7, MinGW‑w64, or MSVC 2017+)
- C++17 is required for `<filesystem>` support

## API Reference
### Constructors
- `Logger(const std::string& loggerName)` — Creates logger with an internal mutex.
- `Logger(const std::string& loggerName, std::mutex& consoleMutex)` — Uses an externally provided mutex.

### Logging Methods
- `logMessage(const std::string& message)` — Regular message (white / default colour)
- `logSuccess(const std::string& message)` — Success message (bright green)
- `logWarning(const std::string& message)` — Warning message (yellow)
- `logError(const std::string& message)` — Error message (bright red)

### File Logging
- `enableFileLogging(const std::string& filePath)` — Start writing logs to a file (parent directories are created automatically).
- `disableFileLogging()` — Stop file logging and close the current file.

## Platform Support
### Windows
- Console colours via the Windows Console API (for all terminals) or ANSI codes (Windows 10+).
- Builds produce:
  - **DLL**: `logger.dll` + import library (`logger.lib` for MSVC, `liblogger.dll.a` for MinGW)
  - **Static**: `logger.lib` (MSVC) or `liblogger.a` (MinGW)

### Linux / macOS
- Console colours via ANSI escape codes.
- Builds produce:
  - **Shared**: `liblogger.so` (Linux) / `liblogger.dylib` (macOS)
  - **Static**: `liblogger.a`

## Building the Library
The library consists of a single header (`Logger.hpp`) and source (`Logger.cpp`) pair.
Compile directly from the command line — no CMake or IDE required.

### Windows (MinGW) static
- `g++ -std=c++17 -I include -c src/Logger/Logger.cpp -DLOGGER_STATIC_DEFINE -o build/Logger.o && ar rcs build/liblogger.a build/Logger.o`

### Windows (MinGW) shared
- `g++ -std=c++17 -I include -c src/Logger/Logger.cpp -DLOGGER_EXPORTS -o build/Logger.o && g++ -shared -o build/logger.dll build/Logger.o -Wl,--out-implib,build/liblogger.dll.a`







