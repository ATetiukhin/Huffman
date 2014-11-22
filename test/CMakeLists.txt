cmake_minimum_required(VERSION 2.8.0)

# Project settings
set(PROJECT_NAME TEST_HUFFMAN)
set(APP_NAME test_huffman)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
message(STATUS "VERSION: " ${VERSION})

project(${PROJECT_NAME})

#-------------------------------------------------------------------------------------------
 
# Source code
set(TEST_FILES test.cpp)

set(ALL_FILES ${TEST_FILES})

#-------------------------------------------------------------------------------------------

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_definitions(/MTd)

if (MSVC AND MSVC_VERSION EQUAL 1700)
  add_definitions(/D _VARIADIC_MAX=10)
endif()

#-------------------------------------------------------------------------------------------

# OS settings
if(WIN32 OR APPLE)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build/install CACHE STRING "" FORCE)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif(WIN32 OR APPLE)

#-------------------------------------------------------------------------------------------

# Project settings
if (MSVC)
    add_definitions(/D_CONSOLE /D_MBCS /wd4512)
    set(CMAKE_CXX_FLAGS "/EHsc /W4 /nologo /Od /Ob0" CACHE STRING "" FORCE)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    # Setting directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIRECTORY} CACHE STRING "" FORCE)

    # Compiler flags
    set(CMAKE_CXX_FLAGS " -std=c++11 -Wall" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Os" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)

    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "" CACHE STRING "" FORCE) # remove dynamic for CXX
    set(BUILD_SHARED_LIBS OFF)
endif (MSVC)

#-------------------------------------------------------------------------------------------

add_executable(${APP_NAME} ${ALL_FILES})
target_link_libraries(${APP_NAME} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

#-------------------------------------------------------------------------------------------