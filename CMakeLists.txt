cmake_minimum_required(VERSION 3.14)
project(Logger VERSION 1.0 LANGUAGES CXX)

# Choose static (default) or shared (DLL) build
option(BUILD_SHARED_LIBS "Build shared library" OFF)

# The library target – named 'Logger'
add_library(Logger
    src/Logger.cpp
)

# Public include path for all users of 'Logger'
target_include_directories(Logger PUBLIC include)

# Logger needs C++17 (std::filesystem, std::chrono, etc.)
target_compile_features(Logger PUBLIC cxx_std_17)

# Optional: help older GCC (≤8) find std::filesystem
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(Logger PUBLIC stdc++fs)
endif()

# When building the shared library, set the correct DLL macros
if(BUILD_SHARED_LIBS)
    target_compile_definitions(Logger PRIVATE LOGGER_DYNAMIC LOGGER_BUILD)
    target_compile_definitions(Logger INTERFACE LOGGER_DYNAMIC)
endif()
