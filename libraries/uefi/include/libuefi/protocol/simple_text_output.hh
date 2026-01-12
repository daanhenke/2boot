#pragma once

#include <libuefi/primitives.hh>

namespace uefi::protocol
{
    struct SimpleTextOutputProtocol
    {
        Status (*reset)(SimpleTextOutputProtocol* self, bool extended_verification);
        Status (*output_string)(SimpleTextOutputProtocol* self, const Char16* string);
    };
}
