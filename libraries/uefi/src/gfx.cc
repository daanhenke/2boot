#include <libuefi/ext/gfx.hh>

#include <libuefi/globals.hh>
#include <libuefi/protocol/boot_logo.hh>
#include <libuefi/ext/string.hh>

namespace uefi
{
    protocol::GraphicsOutputProtocol* gGOP = nullptr;

    bool InitializeGraphics()
    {
        const auto BS = gSystemTable->boot_services;

        if (BS->locate_protocol(&guids::GraphicsOutputProtocolGuid, nullptr, (void**)&gGOP))
        {
            return false;
        }

        return true;
    }

    void ClearScreen(protocol::GraphicsOutputBltPixel color)
    {
        if (!gGOP || !gGOP->mode || !gGOP->mode->info)
            return;

        const auto width = gGOP->mode->info->horizontal_resolution;
        const auto height = gGOP->mode->info->vertical_resolution;

        gGOP->blt(
            gGOP,
            &color,
            protocol::GraphicsOutputBltOperation::VideoFill,
            0, 0,
            0, 0,
            width, height,
            0
        );
    }

    void BlitImageToScreen(const GOPBImage *image, u32 destination_x, u32 destination_y)
    {
        if (!gGOP || !gGOP->mode || !gGOP->mode->info)
            return;

        if (destination_x == static_cast<u32>(-1))
        {
            destination_x = (gGOP->mode->info->horizontal_resolution - image->width) / 2;
        }

        if (destination_y == static_cast<u32>(-1))
        {
            destination_y = (gGOP->mode->info->vertical_resolution - image->height) / 2;
        }

        gGOP->blt(
            gGOP,
            const_cast<protocol::GraphicsOutputBltPixel*>(image->data),
            protocol::GraphicsOutputBltOperation::BltBufferToVideo,
            0, 0,
            destination_x, destination_y,
            image->width, image->height,
            0
        );
    }

    void SetBootLogo(const GOPBImage *image)
    {
        const auto BS = gSystemTable->boot_services;

        protocol::BootLogoProtocol* boot_logo = nullptr;
        if (BS->locate_protocol(&guids::BootLogoProtocolGuid, nullptr, reinterpret_cast<void**>(&boot_logo)))
        {
            return;
        }

        auto screen_width = gGOP->mode->info->horizontal_resolution;
        auto screen_height = gGOP->mode->info->vertical_resolution;

        auto destination_x = (screen_width - image->width) / 2;
        auto destination_y = (screen_height - image->height) / 2;

        boot_logo->set_boot_logo(boot_logo, &image->data[0], destination_x, destination_y, image->width, image->height);
    }


    const GOPFImage* gFontImage = nullptr;
    void SetFont(const GOPFImage* font_image)
    {
        gFontImage = font_image;
    }

    void BlitGlyph(const Char8 glyph, u32 destination_x, u32 destination_y)
    {
        if (!gGOP || !gGOP->mode || !gGOP->mode->info || !gFontImage)
            return;

        const auto cell_w = gFontImage->cell_width;
        const auto cell_h = gFontImage->cell_height;

        const auto glyph_index = static_cast<u32>(glyph - 0x20);
        const auto glyphs_per_row = 16;

        const auto sx = (glyph_index % glyphs_per_row) * cell_w;
        const auto sy = (glyph_index / glyphs_per_row) * cell_h;

        for (u32 y = 0; y < cell_h; y++)
        {
            for (u32 x = 0; x < cell_w; x++)
            {
                const auto px = sx + x;
                const auto py = sy + y;
                auto pixel = &gFontImage->data[py * gFontImage->width + px];
                if (pixel->reserved == 0) continue;

                gGOP->blt(
                    gGOP,
                    const_cast<protocol::GraphicsOutputBltPixel*>(pixel),
                    protocol::GraphicsOutputBltOperation::BltBufferToVideo,
                    0, 0,
                    destination_x + x, destination_y + y,
                    1, 1,
                    0
                );
            }
        }
    }

    void BlitText(const Char8* text, u32 destination_x, u32 destination_y)
    {
        auto x = destination_x;
        auto y = destination_y;

        while(*text)
        {
            if (*text == '\n')
            {
                x = destination_x;
                y += gFontImage->cell_height;
            }
            else
            {
                BlitGlyph(*text, x, y);
                x += gFontImage->cell_width;
            }
            text++;
        }
    }
}
