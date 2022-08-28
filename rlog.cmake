if (MSVC OR MSYS OR MINGW)
    add_definitions(-DRLOG_FOR_WINDOWS)
endif ()

if (APPLE)
    add_definitions(-DRLOG_FOR_APPLE)
endif ()

if (UNIX AND NOT APPLE)
    add_definitions(-DRLOG_FOR_UNIX)
endif ()

set(RLOG_DIRECTORIES  rlog/ )
set(RLOG_EXECUTABLE_FILES  rlog/rlog.c)