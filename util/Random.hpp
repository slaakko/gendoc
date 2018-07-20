// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_RANDOM_INCLUDED
#define GENDOC_UTIL_RANDOM_INCLUDED
#include <stdint.h>

namespace gendoc { namespace util {

void InitMt(uint32_t seed);

uint32_t Random();
uint64_t Random64();

} } // namespace gendoc::util

#endif // GENDOC_UTIL_RANDOM_INCLUDED
