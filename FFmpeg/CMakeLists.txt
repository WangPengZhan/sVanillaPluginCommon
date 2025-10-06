cmake_minimum_required(VERSION 3.15)

project(FFmpeg 
    VERSION 1.0.0 
    LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_compile_definitions(FFMPEG_DLL_EXPORTS)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} SHARED ${SOURCES})


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog::spdlog
        Util
)
