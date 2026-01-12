#pragma once

#include <libuefi/primitives.hh>

namespace uefi::protocol
{
    struct DevicePathProtocol
    {
        u8 type;
        u8 subtype;
        u8 length[2];
    };

    constexpr DevicePathProtocol NullDevicePath = { .type = 0x7f, .subtype = 0xff, .length = { 4, 0 }, };
}
