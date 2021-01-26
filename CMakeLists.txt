cmake_minimum_required(VERSION 3.15)
project(SAL8 C)

set(CMAKE_C_STANDARD 99)

#add_definitions(-DDEBUG_TRACE_MEMORY_OPS)

file(GLOB SOURCE_FILES
        src/*.h
        src/*.c

        src/**/*.h
        src/**/*.c

        src/sal8/*.h
        src/sal8/*.c

        src/sal8/**/*.h
        src/sal8/**/*.c)

if(TARGET_TYPE STREQUAL "library")
    message("Target Type: library")

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-DTARGET_TYPE_LIBRARY)
    list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/src/main.c$")
    add_library(SAL8 SHARED ${SOURCE_FILES})
else()
    message("Target Type: executable")
    add_executable(SAL8 ${SOURCE_FILES})
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(SAL8 PRIVATE -Wall -Wextra)

    set(CMAKE_C_FLAGS_RELEASE FLAGS "${CMAKE_C_FLAGS} -O3")

    if(ARCH STREQUAL "x86")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(SAL8 PRIVATE -W2)

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} /O2")
endif()