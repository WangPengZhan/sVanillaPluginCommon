cmake_minimum_required(VERSION 3.15)

project(NetWork 
    VERSION 1.0.0 
    LANGUAGES CXX)


find_package(CURL REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog
        CURL::libcurl
)
