add_uefi_application(bootloader
  main.cc
)

target_link_libraries(bootloader PRIVATE
  libcommon
  libuefi
)

set_target_properties(bootloader PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/EFI/BOOT"
  OUTPUT_NAME "BOOTX64"
)
