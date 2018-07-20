// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_PARSING_PARSER_INCLUDED
#define GENDOC_PARSING_PARSER_INCLUDED
#include <gendoc/parsing/Match.hpp>
#include <gendoc/parsing/ParsingData.hpp>
#include <gendoc/parsing/ParsingObject.hpp>
#include <stack>

namespace gendoc { namespace parsing {

class Object
{
public:
    virtual ~Object();
};

typedef std::stack<std::unique_ptr<Object>> ObjectStack;

template<typename ValueType>
class ValueObject: public Object
{
public:
    ValueObject(): value() {}
    ValueObject(const ValueType& value_): value(value_) {}
    ValueObject<ValueType>& operator=(const ValueType& v) { value = v; return *this; }
    operator ValueType() const { return value; }
private:
    ValueType value;
};

class Scanner;

class Parser: public ParsingObject
{
public:
    Parser(const std::u32string& name_, const std::u32string& info_);
    Parser(const std::u32string& name_, const std::u32string& info_, ObjectKind kind_);
    const std::u32string& Info() const { return info; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) = 0;
    virtual bool IsEmptyParser() const { return false; }
    virtual bool IsEmptyWithAction() const { return false; }
    virtual bool IsGroupingParser() const { return false; }
    virtual bool IsGroupingWithAction() const { return false; }
private:
    std::u32string info;
};

} } // namespace gendoc::parsing

#endif // GENDOC_PARSING_PARSER_INCLUDED

