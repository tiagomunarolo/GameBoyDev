# set minimum cmake version
cmake_minimum_required(VERSION 3.10)
# project name
project(GameBoyDevEmulator VERSION 0.1.0 LANGUAGES C CXX)
# set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
# set architecture
set(CMAKE_OSX_ARCHITECTURES arm64)

# Set source directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${SRC_DIR}/includes")
set(LIB_DIR "${SRC_DIR}/lib")

# Find all source files
file(GLOB SRC_FILES "${SRC_DIR}/*.cpp")
file(GLOB LIB_FILES "${LIB_DIR}/*.cpp")

find_package(SDL2_ttf REQUIRED)
find_package(SDL2 REQUIRED)

# set debug flags
set(DEBUG_FLAGS "-g -O0 -DDEBUG -Wall -Wextra")
# Enable debug flags for GCC/Clang
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})

# Set SDL2 paths manually
set(SDL2_INCLUDE_DIRS "/opt/homebrew/include/SDL2")
set(SDL2_LIBRARIES "/opt/homebrew/lib/libSDL2.dylib")
include_directories(${SDL2_INCLUDE_DIRS})

# add compile definitions for UI
add_compile_definitions(UI_ENABLED)

# add compile definitions for CPU
# add_compile_definitions(DEBUG_CPU)

# Add main executable
add_executable(GameBoyDevEmulator ${SRC_FILES})

# Add the library
add_library(GameBoyLib ${LIB_FILES})

# Include directories (for both executable and library)
target_include_directories(GameBoyDevEmulator PRIVATE ${INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})
target_include_directories(GameBoyLib PRIVATE ${INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})

# Link the library with the executable
target_link_libraries(GameBoyDevEmulator PRIVATE GameBoyLib ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
