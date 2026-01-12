#pragma once

#include <libuefi/primitives.hh>
#include <libuefi/boot_services.hh>
#include <libuefi/runtime_services.hh>

#include <libuefi/protocol/simple_text_input.hh>
#include <libuefi/protocol/simple_text_output.hh>

namespace uefi
{
    using SystemTable = struct
    {
        TableHeader header;
        Char16* firmware_vendor;
        u32 firmware_revision;

        Handle console_in_handle;
        protocol::SimpleTextInputProtocol* console_in;
        Handle console_out_handle;
        protocol::SimpleTextOutputProtocol* console_out;
        Handle std_err_handle;
        protocol::SimpleTextOutputProtocol* std_err;

        RuntimeServices* runtime_services;
        BootServices* boot_services;

        u64 number_of_table_entries;
        void* configuration_table;
    };
}
