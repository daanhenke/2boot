add_library(libuefi STATIC
  src/globals.cc
  src/string.cc
  src/fs.cc
)

target_include_directories(libuefi PUBLIC include)
target_link_libraries(libuefi PUBLIC libcommon)
