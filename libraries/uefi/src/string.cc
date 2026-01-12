#include <libcommon/primitives.hh>
#include <libuefi/ext/string.hh>

#include <libuefi/globals.hh>

namespace uefi
{
    u64 String8Length(const Char8* string)
    {
        auto ptr = string;
        while (*ptr) ptr++;
        return ptr - string;
    }

    u64 String16Length(const Char16* string)
    {
        auto ptr = string;
        while (*ptr) ptr++;
        return ptr - string;
    }

    void String8ToString16(const Char8* input_string, Char16* output_string, u64 length)
    {
        for (auto i = 0; i <= length; i++)
        {
            output_string[i] = static_cast<Char16>(input_string[i]);
        }
    }

    void String16ToString8(const Char16* input_string, Char8* output_string, u64 length)
    {
        for (auto i = 0; i <= length; i++)
        {
            output_string[i] = static_cast<Char8>(input_string[i]);
        }
    }

    void PrintString16(const Char16* string)
    {
        gSystemTable->console_out->output_string(gSystemTable->console_out, string);
    }

    void PrintString8(const Char8* string, size_t string_length)
    {
        if (string_length == 0)
        {
            string_length = String8Length(string);
        }
        Char16 buffer[192];
        size_t wi = 0;

        for (size_t i = 0; i < string_length; i++)
        {
            const auto ch = string[i];
            if (ch == '\n') {
                if (wi) {
                    buffer[wi] = 0;
                    PrintString16(buffer);
                    wi = 0;
                }
                PrintString16(L"\r\n");
                continue;
            }

            // Treat as ASCII; replace non-ASCII bytes with '?'
            const auto b = static_cast<u8>(ch);
            const auto wch = (b < 0x80) ? static_cast<Char16>(b) : L'?';

            buffer[wi++] = wch;
            if (wi + 1 >= (sizeof(buffer) / sizeof(buffer[0]))) {
                buffer[wi] = 0;
                PrintString16(buffer);
                wi = 0;
            }
        }

        if (wi) {
            buffer[wi] = 0;
            PrintString16(buffer);
        }
    }

    void WaitForInput()
    {
        gSystemTable->boot_services->wait_for_event(1, &gSystemTable->console_in->wait_for_key, nullptr);
    }

    size_t UnsingedToStringEx(char* buffer, const size_t buffer_size, u64 value, const unsigned base, const bool uppercase) {
        static auto lo = "0123456789abcdef";
        static auto hi = "0123456789ABCDEF";
        const char* digits = uppercase ? hi : lo;

        char tmp[64];
        size_t index = 0;

        if (base < 2 || base > 16) return 0;
        if (value == 0) tmp[index++] = '0';

        while (value && index < sizeof(tmp)) {
            tmp[index++] = digits[value % base];
            value /= base;
        }

        // reverse into out
        size_t n = 0;
        while (n < index && n + 1 < buffer_size) {
            buffer[n] = tmp[index - 1 - n];
            n++;
        }
        if (buffer_size) buffer[n < buffer_size ? n : buffer_size - 1] = 0;
        return n;
    }

    size_t SignedToString(char* buffer, const size_t buffer_size, const s64 value) {
        if (!buffer_size) return 0;
        if (value < 0) {
            buffer[0] = '-';
            const auto uv = static_cast<u64>(-(value + 1)) + 1; // avoid overflow on INT64_MIN
            const auto n = UnsingedToStringEx(buffer + 1, buffer_size - 1, uv, 10, 0);
            return 1 + n;
        }
        return UnsingedToStringEx(buffer, buffer_size, static_cast<u64>(value), 10, 0);
    }

    void PadAndWrite(const char* string, size_t string_length, int width, char pad_character) {
        if (width < 0) width = 0;
        if (static_cast<int>(string_length) < width) {
            int pad = width - static_cast<int>(string_length);
            char padding_buffer[64];
            while (pad > 0) {
                int chunk = pad > static_cast<int>(sizeof(padding_buffer)) ? static_cast<int>(sizeof(padding_buffer)) : pad;
                for (int i = 0; i < chunk; i++) padding_buffer[i] = pad_character;
                PrintString8(padding_buffer, static_cast<size_t>(chunk));
                pad -= chunk;
            }
        }
        PrintString8(string, string_length);
    }

    void VFormatPrint(const char *fmt, va_list ap)
    {
        const char* p = fmt;

        while (*p) {
            // emit literal run until '%'
            const char* lit = p;
            while (*p && *p != '%') p++;
            if (p != lit) PrintString8(lit, static_cast<size_t>(p - lit));
            if (*p == 0) break;

            // parse '%'
            p++; // skip '%'
            if (*p == '%') {
                PrintString8("%", 1);
                p++;
                continue;
            }

            // flags
            char padch = ' ';
            int width = 0;

            if (*p == '0')
            {
                padch = '0';
                p++;
            }

            // width
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }

            // length modifier (very small subset)
            enum { LEN_DEF, LEN_L, LEN_LL, LEN_Z } len = LEN_DEF;
            if (*p == 'l') { len = LEN_L; p++; if (*p == 'l') { len = LEN_LL; p++; } }
            else if (*p == 'z') { len = LEN_Z; p++; }

            char spec = *p ? *p++ : 0;
            char numbuf[96];

            switch (spec) {
            case 's': {
                const char* s = va_arg(ap, const char*);
                if (!s) s = "(null)";
                auto n = String8Length(s);
                PadAndWrite(s, n, width, padch);
            } break;

            case 'S': {
                const Char16* s = va_arg(ap, const Char16*);
                if (!s) s = L"(null)";
                auto n = String16Length(s);
                PrintString16(s);
            } break;

            case 'c': {
                int c = va_arg(ap, int);
                char ch = static_cast<char>(c);
                PadAndWrite(&ch, 1, width, padch);
            } break;

            case 'd':
            case 'i': {
                int64_t v;
                if (len == LEN_LL) v = va_arg(ap, long long);
                else if (len == LEN_L) v = va_arg(ap, long);
                else if (len == LEN_Z) v = va_arg(ap, intptr_t);
                else v = va_arg(ap, int);

                // If padding with zeros, keep '-' in front (common behavior)
                if (const auto n = SignedToString(numbuf, sizeof(numbuf), v);
                    padch == '0' && numbuf[0] == '-' && static_cast<int>(n) < width) {
                    PrintString8("-", 1);
                    PadAndWrite(numbuf + 1, n - 1, width - 1, '0');
                } else {
                    PadAndWrite(numbuf, n, width, padch);
                }
            } break;

            case 'u': {
                uint64_t v;
                if (len == LEN_LL) v = va_arg(ap, unsigned long long);
                else if (len == LEN_L) v = va_arg(ap, unsigned long);
                else if (len == LEN_Z) v = (uint64_t)va_arg(ap, size_t);
                else v = va_arg(ap, unsigned int);

                size_t n = UnsingedToStringEx(numbuf, sizeof(numbuf), v, 10, 0);
                PadAndWrite(numbuf, n, width, padch);
            } break;

            case 'x':
            case 'X': {
                uint64_t v;
                if (len == LEN_LL) v = va_arg(ap, unsigned long long);
                else if (len == LEN_L) v = va_arg(ap, unsigned long);
                else if (len == LEN_Z) v = va_arg(ap, size_t);
                else v = va_arg(ap, unsigned int);

                size_t n = UnsingedToStringEx(numbuf, sizeof(numbuf), v, 16, (spec == 'X'));
                PadAndWrite(numbuf, n, width, padch);
            } break;

            case 'p': {
                // Print pointer as 0x... with fixed width for 64-bit
                const auto v = reinterpret_cast<uintptr_t>(va_arg(ap, void *));
                PrintString8("0x", 2);
                const auto n = UnsingedToStringEx(numbuf, sizeof(numbuf), (uint64_t)v, 16, 0);
                // left-pad with zeros to pointer width
                auto pwidth = (sizeof(void*) * 2);
                PadAndWrite(numbuf, n, pwidth, '0');
            } break;

            default: {
                // Unknown spec: print it literally to help debugging
                PrintString8("%", 1);
                if (spec) PrintString8(&spec, 1);
            } break;
            }
        }
    }

    void FormatPrint(const char *fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        VFormatPrint(fmt, ap);
        va_end(ap);
    }
} // namespace uefi
