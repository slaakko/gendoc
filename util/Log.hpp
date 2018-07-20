// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_LOG_INCLUDED
#define GENDOC_UTIL_LOG_INCLUDED
#include <string>

namespace gendoc { namespace util {

void LogMessage(int logStreamId, const std::string& message);

} } // namespace gendoc::util

#endif // GENDOC_UTIL_LOG_INCLUDED
