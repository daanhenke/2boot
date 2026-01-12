#pragma once

#include <libuefi/primitives.hh>

namespace uefi::guids
{
    constexpr Guid GraphicsOutputProtocolGuid = {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};
}

namespace uefi::protocol
{
    struct GraphicsOutputBltPixel
    {
        u8 blue;
        u8 green;
        u8 red;
        u8 reserved;
    };

    enum class GraphicsOutputBltOperation : u32
    {
        VideoFill,
        VideoToBltBuffer,
        BltBufferToVideo,
        VideoToVideo
    };

    enum class GraphicsPixelFormat : u32
    {
        RedGreenBlueReserved8BitPerColor,
        BlueGreenRedReserved8BitPerColor,
        BitMask,
        BltOnly
    };

    struct PixelBitmask
    {
        u32 red_mask;
        u32 green_mask;
        u32 blue_mask;
        u32 reserved_mask;
    };

    struct GraphicsOutputModeInformation
    {
        u32 version;
        u32 horizontal_resolution;
        u32 vertical_resolution;
        GraphicsPixelFormat pixel_format;
        PixelBitmask pixel_information;
        u32 pixels_per_scan_line;
    };

    struct GraphicsOutputProtocolMode
    {
        u32 max_mode;
        u32 mode;
        GraphicsOutputModeInformation* info;
        u64 size_of_info;
        PhysicalAddress frame_buffer_base;
        u64 frame_buffer_size;
    };

    struct GraphicsOutputProtocol
    {
        Status (*query_mode)(GraphicsOutputProtocol* self, u32 mode_number, u64* size_of_info, GraphicsOutputModeInformation** info);
        Status (*set_mode)(GraphicsOutputProtocol* self, u32 mode_number);
        Status (*blt)(GraphicsOutputProtocol* self, GraphicsOutputBltPixel* blt_buffer, GraphicsOutputBltOperation blt_operation, u64 source_x, u64 source_y, u64 destination_x, u64 destination_y, u64 width, u64 height, u64 delta);
        GraphicsOutputProtocolMode* mode;
    };
}
