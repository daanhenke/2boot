#include <libuefi.hh>
#include <libuefi/ext/string.hh>
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

decltype(uefi::RuntimeServices::get_variable) oGetVariable = nullptr;
uefi::Status GetVariableHook(const uefi::Char16* variable_name, uefi::Guid* vendor_guid, u32* attributes, u64* data_size, void* data)
{
    uefi::PrintString8("GetVariableHook called!\r\n");
    return oGetVariable(variable_name, vendor_guid, attributes, data_size, data);
}

uefi::Status UefiMain(const uefi::Handle imageHandle, uefi::SystemTable* systemTable)
{
    uefi::Initialize(imageHandle, systemTable);
    if (!uefi::InitializeFilesystem())
    {
        uefi::PrintString8("Failed to initialize\r\n");
        return 1;
    }
    uefi::PrintString8("Hello World!\r\n");

    if (!LoadDriver("EFI\\2B\\runtime-driver.efi"))
    {
        uefi::PrintString8("Failed to load runtime-driver.efi\r\n");
    }
    uefi::WaitForInput();

    return 0;
}
