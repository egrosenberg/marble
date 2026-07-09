cmake_minimum_required(VERSION 3.24)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

project(marble)


set(HEADER_FILES
    src/lib.h
    src/lib/math.h
    src/song.h
    src/tracklist.h
    
    src/api/api.h
    src/api/routes/player.h)

set(SOURCE_FILES
    src/index.cpp
    src/song.cpp
    src/tracklist.cpp

    src/api/api.cpp
    src/api/routes/player.cpp
    )

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

if (WIN32)
  target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
endif()