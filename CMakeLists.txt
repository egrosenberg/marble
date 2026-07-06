cmake_minimum_required(VERSION 3.24)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

project(marble)

set(HEADER_FILES
    src/lib.h
    src/lib/math.h
    src/song.h
    src/tracklist.h)

set(SOURCE_FILES
    src/index.cpp
    src/song.cpp
    src/tracklist.cpp
    )

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)