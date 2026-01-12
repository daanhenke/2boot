#pragma once

#include <libuefi/primitives.hh>
#include <libuefi/system_table.hh>

namespace uefi::guids
{
    constexpr Guid LoadedImageProtocolGuid = {0x5b1b31a1, 0x9562, 0x11d2, {0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
}

namespace uefi::protocol
{
    struct LoadedImageProtocol
    {
        u32 revision;
        Handle parent_handle;
        SystemTable* system_table;
        Handle device_handle;
        void* file_path;
        void* reserved;
        u32 load_options_size;
        void* load_options;
        void* image_base;
        u64 image_size;
        MemoryType image_code_type;
        MemoryType image_data_type;
        Status (*unload)(Handle image_handle);
    };
}
