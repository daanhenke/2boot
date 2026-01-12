#pragma once

#include <libuefi/system_table.hh>

namespace uefi
{
    extern SystemTable* gSystemTable;
    extern Handle gImageHandle;

    void Initialize(Handle imageHandle, SystemTable* systemTable);
}
