cmake_minimum_required(VERSION 3.12)
project(rgb_recorder_epoch_timestamp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  # On Windows, we rely on env variables to find install paths
  if(NOT DEFINED ENV{OpenCV_DIR})
    message(FATAL_ERROR "OpenCV_DIR env var not set, please set it to the OpenCV install directory")
  endif()
endif()
set(OpenCV_DIR "$ENV{OpenCV_DIR}\\build" CACHE PATH "OpenCV build directory" FORCE)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable
add_executable(rgb_recorder src/main.cpp)

# Link libraries
target_link_libraries(rgb_recorder ${OpenCV_LIBS})

if(UNIX AND NOT APPLE)
    # Linux specific settings
    target_link_libraries(rgb_recorder pthread)
endif()