// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_PARSING_COMPOSITE_INCLUDED
#define GENDOC_PARSING_COMPOSITE_INCLUDED
#include <gendoc/parsing/Parser.hpp>

namespace gendoc { namespace parsing {

class Visitor;

class UnaryParser : public Parser
{
public:
    UnaryParser(const std::u32string& name_, Parser* child_, const std::u32string& info_);
    Parser* Child() const { return child; }
private:
    Parser* child;
};

class OptionalParser : public UnaryParser
{
public:
    OptionalParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PositiveParser : public UnaryParser
{
public:
    PositiveParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class KleeneStarParser : public UnaryParser
{
public:
    KleeneStarParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ExpectationParser : public UnaryParser
{
public:
    ExpectationParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class TokenParser : public UnaryParser
{
public:
    TokenParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class GroupingParser : public UnaryParser
{
public:
    GroupingParser(Parser* child_);
    bool IsGroupingParser() const override { return true; }
    bool IsGroupingWithAction() const override { return Child()->IsActionParser(); }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class BinaryParser : public Parser
{
public:
    BinaryParser(const std::u32string& name_, Parser* left_, Parser* right_, const std::u32string& info_);
    Parser* Left() const { return left; }
    Parser* Right() const { return right; }
private:
    Parser* left;
    Parser* right;
};

class SequenceParser : public BinaryParser
{
public:
    SequenceParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class AlternativeParser : public BinaryParser
{
public:
    AlternativeParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class DifferenceParser : public BinaryParser
{
public:
    DifferenceParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ExclusiveOrParser : public BinaryParser
{
public:
    ExclusiveOrParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class IntersectionParser : public BinaryParser
{
public:
    IntersectionParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ListParser : public UnaryParser
{
public:
    ListParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    Parser* left;
    Parser* right;
};

} } // namespace gendoc::parsing

#endif // GENDOC_PARSING_COMPOSITE_INCLUDED
