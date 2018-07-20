// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_PARSING_NAMESPACE_INCLUDED
#define GENDOC_PARSING_NAMESPACE_INCLUDED
#include <gendoc/parsing/ParsingObject.hpp>
#include <gendoc/codedom/Declaration.hpp>

namespace gendoc { namespace parsing {

class Namespace: public ParsingObject
{
public:
    Namespace(const std::u32string& name_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    virtual bool IsNamespace() const { return true; }
};

class Scope;

class UsingObject: public ParsingObject
{
public:
    UsingObject(gendoc::codedom::UsingObject* subject_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    gendoc::codedom::UsingObject* Subject() const { return subject; }
private:
    gendoc::codedom::UsingObject* subject;
    std::unique_ptr<gendoc::codedom::UsingObject> ownedSubject;
};

} } // namespace gendoc::parsing

#endif // GENDOC_PARSING_NAMESPACE_INCLUDED
