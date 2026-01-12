set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# COFF target suitable for UEFI PE/COFF images
set(COMMON_COMPILE_FLAGS "--target=x86_64-pc-win32-coff -fuse-ld=lld-link -fno-stack-protector -ffreestanding -mno-red-zone -mno-mmx -mno-sse")

set(CMAKE_C_FLAGS_INIT   "${COMMON_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,/entry:UefiMain -nostdlib")

# Use lld-link explicitly
set(CMAKE_LINKER lld-link)

# Don’t try to run target executables
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Wrappers for efi executables
macro(add_uefi_executable TARGET_NAME SUBSYSTEM)
  add_executable("${TARGET_NAME}" ${ARGN})
  target_link_options("${TARGET_NAME}" PRIVATE "-Wl,/subsystem:${SUBSYSTEM}")
  set_target_properties("${TARGET_NAME}" PROPERTIES
    PREFIX ""
    SUFFIX ".efi"
  )
endmacro()

macro(add_uefi_application TARGET_NAME)
  add_uefi_executable("${TARGET_NAME}" EFI_APPLICATION ${ARGN})
endmacro()

macro(add_uefi_boot_service_driver TARGET_NAME)
  add_uefi_executable("${TARGET_NAME}" EFI_BOOT_SERVICE_DRIVER ${ARGN})
endmacro()

macro(add_uefi_runtime_driver TARGET_NAME)
  add_uefi_executable("${TARGET_NAME}" EFI_RUNTIME_DRIVER ${ARGN})
endmacro()
