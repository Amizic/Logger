# Logger
A simple, cross-platform logging library for C++ with coloured console output and file logging support.

All classes live in the `Logger` namespace, and the header is `Logger.hpp`:

```cpp
#include "Logger.hpp"

Logger::Logger log("ClientLib");
log.logSuccess("Connected");
```

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
  - **DLL**: `libLogger.dll` + import library `libLogger.dll.a` (MinGW) / `Logger.lib` (MSVC)
  - **Static**: `libLogger.a` (MinGW) / `Logger.lib` (MSVC)

### Linux / macOS
- Console colours via ANSI escape codes.
- Builds produce:
  - **Shared**: `libLogger.so` (Linux) / `libLogger.dylib` (macOS)
  - **Static**: `libLogger.a`

## Building the Library
The project uses CMake for clean, cross-platform builds.

### Static build (default)
- `cmake -B build_static -G "MinGW Makefiles"`
- `cmake --build build_static`

### Dynamic build
- `cmake -B build_shared -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=ON`
- `cmake --build build_shared`

### Static Test build
- `g++ -std=c++17 tests/test_logger.cpp -Iinclude -Lbuild_static -lLogger -o tests/test_logger_static.exe`

### Dynamic Test build
- `g++ -std=c++17 tests/test_logger.cpp -Iinclude -DLOGGER_DYNAMIC -Lbuild_shared -lLogger -o tests/test_logger_dynamic.exe`
