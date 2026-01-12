#pragma once

#include <libuefi/primitives.hh>

namespace uefi
{
    bool InitializeFilesystem();
    bool ReadFile(const char* path, void** buffer, u64* size);
}
