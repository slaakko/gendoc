// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/codedom/Object.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace codedom {

using namespace gendoc::util;
using namespace gendoc::unicode;

CppObject::CppObject(const std::u32string& name_): name(name_), isOwned(false)
{
}

CppObject::~CppObject()
{
}

void CppObject::Own(CppObject* object)
{
    if (object)
    {
        if (!object->IsOwned())
        {
            object->SetOwned();
            ownedObjects.push_back(std::unique_ptr<CppObject>(object));
        }
    }
}

std::u32string CppObject::ToString() const
{
    return name;
}

void CppObject::Print(CodeFormatter& formatter)
{
    formatter.Write(ToUtf8(name));
}

} } // namespace gendoc::codedom
