cmake_minimum_required(VERSION 3.15)
project(SAL8 C)

set(CMAKE_C_STANDARD 99)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_RELEASE FLAGS "${CMAKE_C_FLAGS} -O3")

    if(ARCH STREQUAL "x86")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} /O2")
endif()

file(GLOB SOURCE_FILES
        src/*.h
        src/*.c

        src/**/*.h
        src/**/*.c)

#add_definitions(-DDEBUG_TRACE_MEMORY_OPS)

add_executable(SAL8 ${SOURCE_FILES})