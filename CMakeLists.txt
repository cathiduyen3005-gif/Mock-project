cmake_minimum_required(VERSION 3.10)
project(CaroGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Player.cpp
    src/PlayerManager.cpp
    src/Board.cpp
    src/AI.cpp
    src/ReplayManager.cpp
    src/Game.cpp
)

# Header files (for reference)
set(HEADERS
    src/Player.h
    src/PlayerManager.h
    src/Board.h
    src/AI.h
    src/ReplayManager.h
    src/Game.h
)

# Include directories
include_directories(src)

# Add executable
add_executable(CaroGame ${SOURCES} ${HEADERS})
