// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_ERROR_INCLUDED
#define GENDOC_UTIL_ERROR_INCLUDED
#include <stdexcept>

namespace gendoc { namespace util {

#ifdef NDEBUG

#define Assert(expression, message) ((void)0)

#else

#define Assert(expression, message) if (!(expression)) throw std::runtime_error(std::string("assertion failed: ") + message)

#endif

} } // namespace gendoc::util

#endif // GENDOC_UTIL_ERROR_INCLUDED
