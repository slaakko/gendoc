// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/parsing/ParsingData.hpp>

namespace gendoc { namespace parsing {

Context::~Context()
{
}

ParsingData::ParsingData(int numRules)
{
    ruleData.resize(numRules);
}

} } // namespace gendoc::parsing
