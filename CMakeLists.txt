cmake_minimum_required(VERSION 3.16)
project(rlog C)
set(CMAKE_C_STANDARD 11)


add_library(${PROJECT_NAME} rlog.c rlog.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC OR MSYS OR MINGW)
    target_compile_definitions(rlog PUBLIC -DRLOG_FOR_WINDOWS)
endif ()

if (APPLE)
    target_compile_definitions(rlog PUBLIC -DRLOG_FOR_APPLE)
endif ()

if (UNIX AND NOT APPLE)
    target_compile_definitions(rlog PUBLIC -DRLOG_FOR_UNIX)
endif ()


