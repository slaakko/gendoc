// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <stdexcept>

namespace gendoc { namespace dom {

class DomException : std::runtime_error
{
public:        
    DomException(const std::string& message_);
};

} } // namespace gendoc::dom
