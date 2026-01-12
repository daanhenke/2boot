#include <libuefi.hh>
#include <libuefi/ext/string.hh>
#include <libuefi/ext/gfx.hh>
#include <libuefi/protocol/device_path.hh>

bool LoadDriver(const char* path)
{
    const auto BS = uefi::gSystemTable->boot_services;

    void* buffer = nullptr;
    u64 buffer_size = 0;
    if (!uefi::ReadFile(path, &buffer, &buffer_size))
    {
        uefi::PrintString8("Failed to read file\r\n");
        return false;
    }

    uefi::Handle image_handle = nullptr;
    if (const auto status = BS->load_image(false, uefi::gImageHandle, &uefi::protocol::NullDevicePath, buffer, buffer_size, &image_handle))
    {
        uefi::FormatPrint("Failed to load image: %llx\r\n", status);
        BS->free_pool(buffer);
        return false;
    }

    if (BS->start_image(image_handle, nullptr, nullptr))
    {
        uefi::PrintString8("Failed to start image\r\n");
        BS->free_pool(buffer);
        return false;
    }

    BS->free_pool(buffer);
    return true;
}

uefi::Status UefiMain(const uefi::Handle imageHandle, uefi::SystemTable* systemTable)
{
    uefi::Initialize(imageHandle, systemTable);
    if (!uefi::InitializeFilesystem())
    {
        uefi::PrintString8("Failed to initialize\r\n");
        return 1;
    }
    if (!uefi::InitializeGraphics())
    {
        uefi::PrintString8("Failed to initialize graphics\r\n");
        return 1;
    }

    uefi::ClearScreen(uefi::Color(2, 0, 1, 0));

    uefi::GOPFImage* font = nullptr;
    uefi::ReadFile("EFI\\2B\\FONT.GOPF", reinterpret_cast<void**>(&font), nullptr);
    if (font)
    {
        uefi::SetFont(font);
    }

    uefi::GOPBImage* logo = nullptr;
    uefi::ReadFile("EFI\\2B\\LOGO.GOPB", reinterpret_cast<void**>(&logo), nullptr);
    if (logo)
    {
        uefi::BlitText("What's reality? I don't know. When my bird was looking at my computer monitor I thought,\r\n"
            "\"That bird has no idea what he's looking at.\" And yet what does the bird do? Does he panic? \r\n"
            "No, he can't really panic, he just does the best he can. Is he able to live in a world where he's so ignorant?\r\n"
            "Well, he doesn't really have a choice. The bird is okay even though he doesn't understand the world.\r\n"
            "You're that bird looking at the monitor, and you're thinking to yourself, 'I can figure this out.'\r\n"
            "Maybe you have some bird ideas. Maybe that's the best you can do.\r\n", 250, 400);
        uefi::SetBootLogo(logo);
        uefi::gSystemTable->boot_services->free_pool(logo);
    }

    uefi::PrintString8("Loading driver...\r\n");

    if (!LoadDriver("EFI\\2B\\runtime-driver.efi"))
    {
        uefi::PrintString8("Failed to load runtime-driver.efi\r\n");
    }
    uefi::WaitForInput();

    return 0;
}
