#pragma once
#include <unordered_set>

namespace sge
{
    template <typename T>
    struct AssetEntry
    {
        T *resource;
        std::unordered_set<int> ids;
    };
}
