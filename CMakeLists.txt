add_library(rlog rlog.c rlog.h)

if (MSVC OR MSYS OR MINGW)
    target_compile_definitions(rlog PUBLIC -DLOG_FOR_WINDOWS)
endif ()

if (APPLE)
    target_compile_definitions(rlog PUBLIC -DLOG_FOR_APPLE)
endif ()

if (UNIX AND NOT APPLE)
    target_compile_definitions(rlog PUBLIC -DLOG_FOR_UNIX)
endif ()
