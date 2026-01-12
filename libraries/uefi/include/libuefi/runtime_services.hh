#pragma once

#include <libuefi/primitives.hh>

namespace uefi
{
    using RuntimeServices = struct
    {
        TableHeader Header;

        void (*stub_0)();
        void (*stub_1)();
        void (*stub_2)();
        void (*stub_3)();
        void (*stub_4)();
        void (*stub_5)();
        Status (*get_variable)(const Char16* variable_name, Guid* vendor_guid, u32* attributes, u64* data_size, void* data);
        void (*stub_7)();
        Status (*set_variable)(const Char16* variableName, Guid* vendor_guid, u32 attributes, u64 size, void* data);
    };
}
