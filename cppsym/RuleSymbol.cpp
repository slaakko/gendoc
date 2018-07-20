// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/RuleSymbol.hpp>

namespace gendoc { namespace cppsym {

RuleSymbol::RuleSymbol(const Span& span_, const std::u32string& name_) : Symbol(span_, name_)
{
    SetAccess(Specifier::public_);
    ruleBody.reset(new dom::Element(U"span"));
    ruleBody->SetAttribute(U"xml:space", U"preserve");
}

std::u32string RuleSymbol::KindStr()
{
    return U"rule";
}

std::unique_ptr<dom::Element> RuleSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> ruleElement(new dom::Element(U"rule"));
    ruleElement->AppendChild(std::unique_ptr<dom::Node>(ruleBody.release()));
    return ruleElement;
}

} } // namespace gendoc::cppsym
