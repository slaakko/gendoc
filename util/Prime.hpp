// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_PRIME_INCLUDED
#define GENDOC_UTIL_PRIME_INCLUDED
#include <stdint.h>

namespace gendoc { namespace util {

// Returns smallest prime greater than or equal to x

uint64_t NextPrime(uint64_t x);

} } // namespace gendoc::util

#endif // GENDOC_UTIL_PRIME_INCLUDED


