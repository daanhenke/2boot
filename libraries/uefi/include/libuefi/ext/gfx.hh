#pragma once

#include <libuefi/primitives.hh>
#include <libuefi/protocol/graphics_output.hh>

namespace uefi
{
    struct [[gnu::packed]] GOPBImage
    {
        Char8 magic[4]; // 'B','P','O','G'
        u16 version;
        u16 flags;
        u32 width;
        u32 height;
        protocol::GraphicsOutputBltPixel data[];
    };
    static_assert(sizeof(GOPBImage) == 16, "Oh no, packing is broken!");

    struct [[gnu::packed]] GOPFImage
    {
        Char8 magic[4]; // 'F','P','O','G'
        u16 version;
        u16 flags;
        u32 width;
        u32 height;
        u16 cell_width;
        u16 cell_height;
        protocol::GraphicsOutputBltPixel data[];
    };
    static_assert(sizeof(GOPFImage) == 20, "Oh no, packing is broken!");

    constexpr protocol::GraphicsOutputBltPixel Color(u8 r, u8 g, u8 b, u8 a = 0)
    {
        protocol::GraphicsOutputBltPixel color;
        color.red = r;
        color.green = g;
        color.blue = b;
        color.reserved = a;
        return color;
    }

    bool InitializeGraphics();
    void ClearScreen(protocol::GraphicsOutputBltPixel color);
    void BlitImageToScreen(const GOPBImage* image, u32 destination_x = -1, u32 destination_y = -1);
    void SetBootLogo(const GOPBImage* image);
    void SetFont(const GOPFImage* font_image);
    void BlitText(const Char8* text, u32 destination_x = -1, u32 destination_y = -1);
}
