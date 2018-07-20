// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/xpath/InitDone.hpp>
#include <gendoc/xpath/XPathFunction.hpp>
#include <gendoc/xpath/XPathDebug.hpp>

namespace gendoc { namespace xpath {

void Init()
{
    InitFunction();
    InitDebug();
}

void Done()
{
    DoneDebug();
    DoneFunction();
}

} } // namespace gendoc::xpath
