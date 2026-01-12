add_library(libcommon STATIC
    src/memory.cc
)

target_include_directories(libcommon PUBLIC include)
