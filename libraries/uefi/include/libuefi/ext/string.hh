#pragma once

#include <libuefi/primitives.hh>

#include <stdarg.h>

namespace uefi
{
    u64 String8Length(const Char8* string);
    u64 String16Length(const Char16* string);

    void String8ToString16(const Char8* input_string, Char16* output_string, u64 length);
    void String16ToString8(const Char16* input_string, Char8* output_string, u64 length);

    void PrintString8(const Char8* string, size_t string_length = 0);
    void PrintString16(const Char16* string);
    void VFormatPrint(const char* fmt, va_list ap);
    void FormatPrint(const char* fmt, ...);
    void WaitForInput();
}
