cmake_minimum_required(VERSION 3.14)
project(LoggerLibrary VERSION 1.0 LANGUAGES CXX)

# Choose static (default) or shared (DLL) build
option(BUILD_SHARED_LIBS "Build shared library" OFF)

# The library target – named 'logger'
add_library(logger
    src/LoggerHandler.cpp
)

# Public include path for all users of 'logger'
target_include_directories(logger PUBLIC include)

# Logger needs C++17 (std::filesystem, std::chrono, etc.)
target_compile_features(logger PUBLIC cxx_std_17)

# Optional: help older GCC (≤8) find std::filesystem
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(logger PUBLIC stdc++fs)
endif()

# When building the shared library, set the correct DLL macros
if(BUILD_SHARED_LIBS)
    target_compile_definitions(logger PRIVATE LOGGER_DYNAMIC LOGGER_BUILD)
    target_compile_definitions(logger INTERFACE LOGGER_DYNAMIC)
endif()