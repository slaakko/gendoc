// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_UTIL_INCLUDED
#define GENDOC_UTIL_UTIL_INCLUDED
#include <vector>
#include <stdint.h>
#include <string>

namespace gendoc { namespace util {

template<typename StringT>
std::vector<StringT> Split(const StringT& s, typename StringT::value_type c)
{
    std::vector<StringT> v;
    int start = 0;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        if (s[i] == c)
        {
            v.push_back(s.substr(start, i - start));
            start = i + 1;
        }
    }
    if (start < n)
    {
        v.push_back(s.substr(start, n - start));
    }
    return v;
}

inline uint64_t Align(uint64_t n, uint64_t alignment)
{
    return (n + alignment - 1u) & -alignment;
}

} } // namespace gendoc::util

#endif // GENDOC_UTIL_UTIL_INCLUDED
