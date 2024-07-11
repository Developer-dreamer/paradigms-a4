cmake_minimum_required(VERSION 3.11) # Specify the minimum version for CMake

project(paradigms-a4) # Define the project name

set(CMAKE_CXX_STANDARD 17) # Set C++ standard
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

include_directories(headers)

# Add the executable
add_executable(paradigms-a4
        main.cpp
        source/ConsoleProcessor.cpp
        source/ProgramBackup.cpp
        source/TextProcessor.cpp
        source/CaesarAlgorythm.cpp
        )

# If you have additional libraries to include, use target_link_libraries
# target_link_libraries(paradigms-a2 someLibrary)