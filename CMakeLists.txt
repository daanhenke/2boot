cmake_minimum_required(VERSION 3.30)

project(
    2b
    VERSION "0.0.0"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CxxStandardDefs)

add_subdirectory(libraries/common)

if(TWOBOOT_TOOLCHAIN STREQUAL "uefi")
  add_subdirectory(libraries/uefi)
  add_subdirectory(bootloader)
  add_subdirectory(runtime-driver)
endif()
