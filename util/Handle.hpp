// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_UTIL_HANDLE_INCLUDED
#define GENDOC_UTIL_HANDLE_INCLUDED
#if defined(_WIN32)
#include <io.h>
#elif defined(__linux) || defined(__posix) || defined(__unix)
#include <unistd.h>
#else
#error unknown platform
#endif

namespace gendoc { namespace util {

class Handle
{
public:
    Handle(int handle_) : handle(handle_)
    {
    }
    ~Handle()
    {
        if (handle != -1)
        {
            close(handle);
        }
    }
    Handle(Handle&& that)
    {
        handle = that.handle;
        that.handle = -1;
    }
    void operator=(Handle&& that)
    {
        if (handle != -1)
        {
            close(handle);
        }
        handle = that.handle;
        that.handle = -1;
    }
    operator int() const { return handle; }
private:
    int handle;
};

} } // namespace gendoc::util

#endif // GENDOC_UTIL_HANDLE_INCLUDED
