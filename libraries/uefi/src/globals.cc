#include <libuefi/globals.hh>

namespace uefi
{
    SystemTable* gSystemTable = nullptr;
    Handle gImageHandle = nullptr;

    void Initialize(Handle imageHandle, SystemTable* systemTable)
    {
        gImageHandle = imageHandle;
        gSystemTable = systemTable;
    }
}
