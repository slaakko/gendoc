// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/util/Log.hpp>
#include <iostream>
#include <mutex>

namespace gendoc { namespace util {

std::mutex logMutex;

void LogMessage(int logStreamId, const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (logStreamId == -1)
    {
        std::cout << message << std::endl;
    }
    else
    {
        std::cout << logStreamId << ">" << message << std::endl;
    }
}

} } // namespace gendoc::util
