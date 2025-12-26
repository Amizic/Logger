cmake_minimum_required(VERSION 3.10)
project(Logger)

set(CMAKE_CXX_STANDARD 17)

# Build both static and shared libraries
add_library(Logger_static STATIC src/Logger/Logger.cpp)
add_library(Logger_shared SHARED src/Logger/Logger.cpp)

message(STATUS "Building both STATIC and SHARED libraries")

# Set proper output names
set_target_properties(Logger_static PROPERTIES OUTPUT_NAME Logger)
set_target_properties(Logger_shared PROPERTIES OUTPUT_NAME Logger)

# Apply common settings to both libraries
foreach(target Logger_static Logger_shared)
    target_include_directories(${target} PUBLIC include)
    if(WIN32)
        target_link_libraries(${target} PRIVATE kernel32)
    endif()
endforeach()

# Apply shared library specific settings
if(WIN32)
    target_compile_definitions(Logger_shared PRIVATE LOGGER_EXPORTS)
endif()

# Install both libraries
install(TARGETS Logger_static Logger_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
