cmake_minimum_required(VERSION 3.16)
project(rlog C)
set(CMAKE_C_STANDARD 11)

option(RLOG_USE_PRECISE_TIMESTAMPS "Use precise timestamps (affects Apple, Unix, and Windows platforms)" OFF)

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

if (RLOG_USE_PRECISE_TIMESTAMPS)
    target_compile_definitions(rlog PRIVATE RLOG_USE_PRECISE_TIMESTAMPS)
endif()


#################### FORMAT SECTION #######################

set(FILES_FOR_FORMATTING rlog.c rlog.h )

if (NOT TARGET rlibhelper)
    include(FetchContent)
    FetchContent_Declare(
            rlibhelper
            GIT_REPOSITORY https://github.com/RoboticsHardwareSolutions/rlibhelper.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(rlibhelper)
endif ()

if(EXISTS "${rlibhelper_SOURCE_DIR}/format.cmake")
    include(${rlibhelper_SOURCE_DIR}/format.cmake)
else()
    message(WARNING "format.cmake file not found")
endif()
format_files(${PROJECT_NAME} SOURCES ${FILES_FOR_FORMATTING})
