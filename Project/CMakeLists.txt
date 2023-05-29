cmake_minimum_required(VERSION 3.15)

project(
    MPCS51045_Project
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG v3.4.1
)
FetchContent_MakeAvailable(ctre)

add_subdirectory(img)
add_subdirectory(transformations)

find_package(Boost 1.70.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Boost::boost
    img
    transformations
    ctre::ctre
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)