// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/dom/Exception.hpp>

namespace gendoc { namespace dom {

DomException::DomException(const std::string& message_) : std::runtime_error(message_)
{
}

} } // namespace gendoc::dom
