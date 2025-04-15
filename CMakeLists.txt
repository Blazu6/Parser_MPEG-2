cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(PROJECT_NAME "TS-PARSER")
project(${PROJECT_NAME})

# force static runtime libraries for msvc builds
if(MSVC)
  set(variables CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
  foreach(variable ${variables})
  if(${variable} MATCHES "/MD")
    string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
  endif()
  endforeach()
endif()

# set c++17
set (CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# compile everything position independent (even static libraries)
set( CMAKE_POSITION_INDEPENDENT_CODE TRUE )

# set verbose compile options
#set( CMAKE_VERBOSE_MAKEFILE ON )

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /PROFILE")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()


include_directories(include)

# znajdź wszystkie pliki .cpp w src/
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# znajdź wszystkie pliki .h/.hpp w include/
file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)

set(PROJECT_SOURCES ${SRC_FILES} ${HEADER_FILES})

# grupowanie plików np. w IDE
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADER_FILES})

# build
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})


