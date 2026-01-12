#pragma once

#include <libuefi/primitives.hh>

namespace uefi::protocol
{
    struct InputKey
    {
        u16 scan_code;
        Char16 unicode_char;
    };

    struct SimpleTextInputProtocol
    {
        Status (*reset)(SimpleTextInputProtocol* self, bool extended_verification);
        Status (*read_key_stroke)(SimpleTextInputProtocol* self, InputKey* key);
        Event wait_for_key;
    };
}
