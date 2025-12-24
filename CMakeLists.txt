cmake_minimum_required(VERSION 3.10)
project(Logger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SIMPLE OPTION: Let user choose library type
option(BUILD_SHARED_LIBS "Build as shared library (.dll/.so)" OFF)
option(BUILD_STATIC_LIBS "Build as static library (.lib/.a)" ON)

# Create the library based on user choice
if(BUILD_SHARED_LIBS)
    add_library(Logger SHARED src/Logger/Logger.cpp)
    message(STATUS "Building as SHARED library (.dll/.so)")
    
    # Windows DLL export settings
    if(WIN32)
        target_compile_definitions(Logger PRIVATE LOGGER_EXPORTS)
    endif()
elseif(BUILD_STATIC_LIBS)
    add_library(Logger STATIC src/Logger/Logger.cpp)
    message(STATUS "Building as STATIC library (.lib/.a)")
else()
    # Default to static if nothing specified
    add_library(Logger STATIC src/Logger/Logger.cpp)
    message(STATUS "Building as STATIC library (default)")
endif()

# Include directories
target_include_directories(Logger PUBLIC include)

# Platform-specific settings
if(WIN32)
    target_link_libraries(Logger PRIVATE kernel32)
else()
    # Linux/macOS doesn't need special libraries
endif()

# Optional: Install instructions (if user wants to install)
install(TARGETS Logger
    LIBRARY DESTINATION lib      # .dll/.so files
    ARCHIVE DESTINATION lib      # .lib/.a files
    RUNTIME DESTINATION bin      # .dll on Windows
    INCLUDES DESTINATION include # header files
)

install(DIRECTORY include/ DESTINATION include)