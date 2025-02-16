cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Alpha_Game)

# Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find all .cpp files automatically
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Include directories
include_directories(libs/stb src)

# Create the executable
add_executable(Alpha_Game WIN32 ${SRC_FILES})

# Link Windows libraries (for OpenGL & WinAPI)
target_link_libraries(Alpha_Game user32 gdi32 opengl32)
