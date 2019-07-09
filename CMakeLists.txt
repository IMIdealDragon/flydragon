cmake_minimum_required(VERSION 2.6)

project(flydragon C CXX)

add_compile_options(-std=c++11)
include_directories(${PROJECT_SOURCE_DIR}/_include)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  MESSAGE(STATUS "current platform: Windows")
  include_directories("D:\\linux_shared\\include")
  include_directories("D:\\linux_shared\\include\\x86_64-linux-gnu")
  include_directories("D:\\linux_shared\\include\\x86_64-linux-gnu\\sys")
endif ()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

#export()

set(CXX_FLAGS
 -g
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 -rdynamic
 )

if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND CXX_FLAGS "-Wno-null-dereference")
  list(APPEND CXX_FLAGS "-Wno-sign-conversion")
  list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
  list(APPEND CXX_FLAGS "-Wthread-safety")
  list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)



include_directories(${Boost_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/_include)
MESSAGE(${PROJECT_SOURCE_DIR}/_include)
MESSAGE(${CXX_FLAGS})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

add_subdirectory(app)
add_subdirectory(misc)
add_subdirectory(proc)
add_subdirectory(signal)
add_subdirectory(net)




