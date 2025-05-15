cmake_minimum_required(VERSION 3.15)
project(HelloC C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add code coverage option
option(CODE_COVERAGE "Enable coverage reporting" OFF)

# Automatically enable coverage for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CODE_COVERAGE ON)
endif()

# Configure coverage flags
if(CODE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

# Enable warnings and treat them as errors
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Enable testing
enable_testing()

# Add subdirectories for source and tests
add_subdirectory(src)
add_subdirectory(test)

# Create main executable
add_executable(publisher_demo src/main.c)
target_link_libraries(publisher_demo publisher)
target_include_directories(publisher_demo PRIVATE include)
