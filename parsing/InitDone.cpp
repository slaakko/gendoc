// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/parsing/InitDone.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/codedom/InitDone.hpp>

namespace gendoc { namespace parsing {

void Init()
{
    gendoc::codedom::Init();
    ParsingDomainInit();
    KeywordInit();
}

void Done()
{
    KeywordDone();
    ParsingDomainDone();
    gendoc::codedom::Done();
}

} } // namespace gendoc::parsing

