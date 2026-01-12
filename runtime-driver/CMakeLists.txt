add_uefi_runtime_driver(runtime-driver
    main.cc
)

target_link_libraries(runtime-driver PRIVATE
    libcommon
    libuefi
)

set_target_properties(runtime-driver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/EFI/2B"
    OUTPUT_NAME "runtime-driver"
)
