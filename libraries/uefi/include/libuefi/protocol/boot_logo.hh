#pragma once

#include <libuefi/primitives.hh>
#include <libuefi/protocol/graphics_output.hh>

namespace uefi::guids
{
    constexpr Guid BootLogoProtocolGuid = {0xcdea2bd3, 0xfc25, 0x4c1c, {0xb9, 0x7c, 0xb3, 0x11, 0x86, 0x6, 0x49, 0x90}};
}

namespace uefi::protocol
{
    struct BootLogoProtocol
    {
        Status (*set_boot_logo)(BootLogoProtocol* self, const GraphicsOutputBltPixel* blt_buffer, u64 destination_x, u64 destination_y, u64 width, u64 height);
    };
}
