cmake_minimum_required(VERSION 3.10)
project(DAGit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SOURCE IDENTIFICATION ---
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# --- EXECUTABLE & INCLUDE PATHS ---
add_executable(dagit ${SOURCES} main.cpp)

target_include_directories(dagit PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/CLI"
)

# --- LINKING ---
find_package(OpenSSL REQUIRED)

target_link_libraries(dagit PRIVATE OpenSSL::Crypto)