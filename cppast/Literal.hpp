// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_LITERAL_INCLUDED
#define GENDOC_CPPAST_LITERAL_INCLUDED
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppast {

enum class Suffix : uint8_t
{
    none = 0, 
    f = 1 << 0, 
    l = 1 << 1,
    ll = 1 << 2, 
    u = 1 << 3
};

inline Suffix operator|(Suffix left, Suffix right)
{
    return Suffix(uint8_t(left) | uint8_t(right));
}

inline Suffix operator&(Suffix left, Suffix right)
{
    return Suffix(uint8_t(left) & uint8_t(right));
}

enum class Base
{
    decimal, hex, octal
};

std::u32string ToString(Suffix suffix);

class LiteralNode : public Node
{
public:
    LiteralNode(const Span& span_, const std::u32string& rep_);
    const std::u32string& Rep() const { return rep; }
private:
    std::u32string rep;
};

class FloatingLiteralNode : public LiteralNode
{
public:
    FloatingLiteralNode(const Span& span_, double value_, Suffix suffix_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
    double Value() const { return value; }
    Suffix GetSuffix() const { return suffix; }
private:
    double value;
    Suffix suffix;
};

class IntegerLiteralNode : public LiteralNode
{
public:
    IntegerLiteralNode(const Span& span_, uint64_t value_, Suffix suffix_, Base base_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
    uint64_t Value() const { return value; }
    Suffix GetSuffix() const { return suffix; }
    Base GetBase() const { return base; }
private:
    uint64_t value;
    Suffix suffix;
    Base base;
};

class CharacterLiteralNode : public LiteralNode
{
public:
    CharacterLiteralNode(const Span& span_, char32_t prefix_, const std::u32string& chars_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
    char32_t Prefix() const { return prefix; }
    const std::u32string& Chars() const { return chars; }
private:
    char32_t prefix;
    std::u32string chars;
};

class StringLiteralNode : public LiteralNode
{
public:
    StringLiteralNode(const Span& span_, const std::u32string& encodingPrefix_, const std::u32string& chars_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
    const std::u32string& EncodigPrefix() const { return encodingPrefix; }
    const std::u32string& Chars() const { return chars; }
private:
    std::u32string encodingPrefix;
    std::u32string chars;
};

class BooleanLiteralNode : public LiteralNode
{
public:
    BooleanLiteralNode(const Span& span_, bool value_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
    bool Value() const { return value; }
private:
    bool value;
};

class NullPtrLiteralNode : public LiteralNode
{
public:
    NullPtrLiteralNode(const Span& span_, const std::u32string& rep_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_LITERAL_INCLUDED
