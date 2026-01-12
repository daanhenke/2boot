#include <libuefi.hh>
#include <libuefi/ext/string.hh>
#include <libcommon/hooks.hh>

decltype(uefi::RuntimeServices::get_variable) oGetVariable = nullptr;
uefi::Status GetVariableHook(const uefi::Char16* variable_name, uefi::Guid* vendor_guid, u32* attributes, u64* data_size, void* data)
{
    uefi::FormatPrint("GetVariable hooked for var '%S'\r\n", variable_name);
    return oGetVariable(variable_name, vendor_guid, attributes, data_size, data);
}

uefi::Status UefiMain(const uefi::Handle image_handle, uefi::SystemTable* system_table)
{
    uefi::Initialize(image_handle, system_table);
    uefi::PrintString8("Hello from driver!\r\n");

    hooks::SwapPointers(
        &uefi::gSystemTable->runtime_services->get_variable,
        &oGetVariable,
        GetVariableHook
    );
    return 0;
}
