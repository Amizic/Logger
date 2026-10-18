# Logger

A simple, cross-platform logging library for C++ with coloured console
output and file logging support. One class, one header:

```cpp
#include "Logger.hpp"   // namespace Logger
```

## Features

- Coloured console output (Windows, Linux, macOS)
- File logging with automatic directory creation
- Thread-safe logging with configurable mutexes
- Timestamped messages with millisecond precision
- Static or shared library builds
- C++17 (GCC >= 8, Clang >= 7, MinGW-w64, or MSVC 2017+)

## API reference

### Logger

#### `Logger::Logger(loggerName)`
Creates a logger with its own internal mutex.
- **In:** `loggerName` — `std::string`, printed as the log source tag.
- **Returns:** nothing (constructor).

#### `Logger::Logger(loggerName, consoleMutex)`
Creates a logger that shares an externally provided console mutex — use this
when several loggers print to the same console and their lines must not
interleave.
- **In:** `loggerName` — tag; `consoleMutex` — `std::mutex&` owned by you, must outlive the logger.
- **Returns:** nothing (constructor).

#### `Logger::~Logger()`
Closes the log file (if any) and releases resources.

#### `Logger::enableFileLogging(filePath)`
Starts writing every log line to a file as well. Parent directories are
created automatically.
- **In:** `filePath` — `std::string`, the file to append to.
- **Returns:** nothing (call `logMessage` etc. after this to see file output).

#### `Logger::disableFileLogging()`
Stops file logging and closes the current file. Console output continues.
- **Returns:** nothing.

#### `Logger::logMessage(message)`
Logs a regular message (white / default colour), with a timestamp and the
logger's tag.
- **In:** `message` — `std::string`, the text.
- **Returns:** nothing.

#### `Logger::logSuccess(message)`
Logs a success message in bright green.
- **In / Returns:** as above.

#### `Logger::logWarning(message)`
Logs a warning message in yellow.
- **In / Returns:** as above.

#### `Logger::logError(message)`
Logs an error message in bright red.
- **In / Returns:** as above.

## Quick start

```cpp
#include "Logger.hpp"

Logger::Logger log("ClientLib");
log.logSuccess("Connected");
log.logWarning("Retrying in 5 s");
log.logError("Connection lost");
log.enableFileLogging("logs/client.log");
log.logMessage("This line also goes to the file");
```

## Platform support

### Windows
- Console colours via the Windows Console API.
- Builds produce:
  - **DLL**: `libLogger.dll` + import library `libLogger.dll.a` (MinGW) / `Logger.lib` (MSVC)
  - **Static**: `libLogger.a` (MinGW) / `Logger.lib` (MSVC)

### Linux / macOS
- Console colours via ANSI escape codes.
- Builds produce:
  - **Shared**: `libLogger.so` (Linux) / `libLogger.dylib` (macOS)
  - **Static**: `libLogger.a`

## Building the library

The project uses CMake for clean, cross-platform builds.

### Static build (default)
- `cmake -B build_static -G "MinGW Makefiles"`
- `cmake --build build_static`

### Dynamic build
- `cmake -B build_shared -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=ON`
- `cmake --build build_shared`

### Static test build
- `g++ -std=c++17 tests/test_logger.cpp -Iinclude -Lbuild_static -lLogger -o tests/test_logger_static.exe`

### Dynamic test build
- `g++ -std=c++17 tests/test_logger.cpp -Iinclude -DLOGGER_DYNAMIC -Lbuild_shared -lLogger -o tests/test_logger_dynamic.exe`
