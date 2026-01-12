#pragma once

namespace hooks
{
    template <typename T>
    void SwapPointers(T* pointer, T* original, T replacement)
    {
        if (pointer == nullptr)
        {
            return;
        }

        if (original != nullptr)
        {
            *original = *pointer;
        }

        *pointer = replacement;
    }
}
