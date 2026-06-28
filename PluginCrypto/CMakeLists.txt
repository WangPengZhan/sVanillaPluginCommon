cmake_minimum_required(VERSION 3.15)

project(PluginCrypto
    VERSION 1.0.0
    DESCRIPTION "sVanilla plugin cryptography helpers"
    LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        OpenSSL::Crypto
)
