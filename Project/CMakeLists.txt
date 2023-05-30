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
add_subdirectory(filters)
add_subdirectory(utils)

find_package(Boost 1.70.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Boost::boost
    ctre::ctre
    img
    filters
    utils
)

find_package(JPEG REQUIRED)
if (JPEG_FOUND)
  include_directories(${JPEG_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} PRIVATE ${JPEG_LIBRARIES})
endif (JPEG_FOUND)

find_package(PNG REQUIRED)
if (PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIRS})
  target_link_libraries (${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})
endif (PNG_FOUND)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)