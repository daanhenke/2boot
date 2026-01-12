#pragma once

#include <libcommon/primitives.hh>

namespace uefi
{
    using Status = u64;
    namespace status
    {
        constexpr Status Success = 0x0;
        constexpr Status LoadError = 0x8000000000000001;
        constexpr Status InvalidParameter = 0x8000000000000002;
        constexpr Status Unsupported = 0x8000000000000003;
        constexpr Status BadBufferSize = 0x8000000000000004;
        constexpr Status BufferTooSmall = 0x8000000000000005;
    };

    using Handle = void*;
    using Event = void*;

    using PhysicalAddress = u64;
    using VirtualAddress = u64;

    using Char8 = char;
    using Char16 = wchar_t;

    struct Guid
    {
        u32 ms1;
        u16 ms2;
        u16 ms3;
        u8 ms4[8];
    };

    struct Time
    {
        u16 year;
        u8 month;
        u8 day;
        u8 hour;
        u8 minute;
        u8 second;
        u8 pad1;
        u32 nanosecond;
        s16 time_zone;
        u8 daylight;
        u8 pad2;
    };

    struct TableHeader
    {
        u64 signature;
        u32 revision;
        u32 header_size;
        u32 crc32;
        u32 reserved;
    };
}
