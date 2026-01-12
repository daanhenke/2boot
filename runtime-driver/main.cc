#include <libuefi.hh>

#include "libuefi/ext/string.hh"

uefi::Status UefiMain(const uefi::Handle image_handle, uefi::SystemTable* system_table)
{
    uefi::Initialize(image_handle, system_table);
    uefi::PrintString8("Hello from driver!\r\n");
    return 0;
}
