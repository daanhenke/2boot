#pragma once

#include <libuefi/primitives.hh>
#include <libuefi/protocol/file.hh>

namespace uefi::guids
{
    constexpr Guid SimpleFilesystemProtocolGuid = {0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
}

namespace uefi::protocol
{
    struct SimpleFilesystemProtocol
    {
        u64 revision;
        Status (*open_volume)(SimpleFilesystemProtocol* self, FileProtocol** root);
    };
}
