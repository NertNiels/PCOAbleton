cmake_minimum_required(VERSION 3.20)
project(pcoableton)

set(CMAKE_CXX_STANDARD 17)

# Set to C++ 11 if you are using cpr <= 1.9.x
# More: https://github.com/libcpr/cpr#supported-releases
# set(CMAKE_CXX_STANDARD 11)

# Set a default build type if none was specified
# Based on: https://github.com/openchemistry/tomviz/blob/master/cmake/BuildType.cmake
set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(DEFAULT_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# CPR
add_subdirectory(external/cpr)

# NLOHMANN JSON
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBRARY ON CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLEW
add_subdirectory(external/glew)

# IMGUI
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)
add_subdirectory(external/imgui)

# PCOABLETON
add_subdirectory(pcoableton)

# PCOABLETON CONSOLE
add_subdirectory(pcoableton_console)