#pragma once

#include <libuefi/primitives.hh>

namespace uefi::guids
{
    constexpr Guid FileInfoGuid = {0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
    constexpr Guid FilesystemInfoGuid = {0x09576e93, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
    constexpr Guid FilesystemVolumeLabelGuid = {0xdb47d7d3, 0xfe81, 0x11d3, {0x9a, 0x35, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
}

namespace uefi::protocol
{
    enum class FileOpenMode
    {
        Create,
        Read,
        Write
    };

    struct FileInfo
    {
        u64 size;
        u64 file_size;
        u64 physical_size;
        Time create_time;
        Time last_access_time;
        Time modification_time;
        u64 attribute;
        Char16 file_name[];
    };

    struct FileProtocol
    {
        u64 revision;
        Status (*open)(FileProtocol* self, FileProtocol** new_handle, Char16* file_name, FileOpenMode open_mode, u64 attributes);
        Status (*close)(FileProtocol* self);
        Status (*delete_file)(FileProtocol* self);
        Status (*read)(FileProtocol* self, u64* buffer_size, void* buffer);
        Status (*write)(FileProtocol* self, u64* buffer_size, void* buffer);
        Status (*get_position)(FileProtocol* self, u64* position);
        Status (*set_position)(FileProtocol* self, u64 position);
        Status (*get_info)(FileProtocol* self, const Guid* info_type, u64* buffer_size, void* buffer);
    };
}
