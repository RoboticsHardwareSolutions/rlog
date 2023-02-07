if (MSVC OR MSYS OR MINGW)
    add_definitions(-DRLOG_FOR_WINDOWS)
endif ()

if (APPLE)
    add_definitions(-DRLOG_FOR_APPLE)
endif ()

if (UNIX AND NOT APPLE)
    add_definitions(-DRLOG_FOR_UNIX)
endif ()

set(RLOG_DIRECTORIES  ${CMAKE_CURRENT_LIST_DIR} )
set(RLOG_EXECUTABLE_FILES  ${CMAKE_CURRENT_LIST_DIR}/rlog.c)