// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/util/InitDone.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace util {

void Init()
{
    gendoc::unicode::UnicodeInit();
}

void Done()
{
    gendoc::unicode::UnicodeDone();
}

} } // namespace gendoc::util

