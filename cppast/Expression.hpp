// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_EXPRESSION_INCLUDED
#define GENDOC_CPPAST_EXPRESSION_INCLUDED
#include <gendoc/cppast/SimpleType.hpp>

namespace gendoc { namespace cppast {

enum class Operator : uint8_t
{
    assign, mulAssign, divAssign, remAssign, addAssign, subAssign, shiftRightAssign, shiftLeftAssign, andAssign, xorAssign, orAssign,
    equal, notEqual, lessOrEqual, greaterOrEqual, less, greater, shiftLeft, shiftRight, add, sub, mul, div, rem, dotStar, arrowStar, arrow, 
    inc, dec, deref, addrOf, unaryPlus, unaryMinus, not_, complement, sizeOf, alignOf, dynamicCast, staticCast, reinterpretCast, constCast,
    logicalAnd, logicalOr, xor_, and_, or_, apply, subscript, comma, newArray, deleteArray, new_, delete_
};

std::u32string OpStr(Operator op);

class ExpressionSequenceNode : public BinaryNode
{
public:
    ExpressionSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class CommaExpressionNode : public BinaryNode
{
public:
    CommaExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class AssignmentExpressionNode : public BinaryNode
{
public:
    AssignmentExpressionNode(const Span& span_, Node* left_, Operator op_, Node* right_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class ConditionalExpressionNode : public Node
{
public:
    ConditionalExpressionNode(const Span& span_, Node* condition_, Node* thenExpr_, Node* elseExpr_);
    void Accept(Visitor& visitor) override;
    Node* Condition() { return condition.get(); }
    Node* ThenExpr() { return thenExpr.get(); }
    Node* ElseExpr() { return elseExpr.get(); }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> thenExpr;
    std::unique_ptr<Node> elseExpr;
};

class ThrowExpressionNode : public UnaryNode
{
public:
    ThrowExpressionNode(const Span& span_, Node* exceptionExpr_);
    void Accept(Visitor& visitor) override;
};

class LogicalOrExpressionNode : public BinaryNode
{
public:
    LogicalOrExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class LogicalAndExpressionNode : public BinaryNode
{
public:
    LogicalAndExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class InclusiveOrExpressionNode : public BinaryNode
{
public:
    InclusiveOrExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class ExclusiveOrExpressionNode : public BinaryNode
{
public:
    ExclusiveOrExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class AndExpressionNode : public BinaryNode
{
public:
    AndExpressionNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class EqualityExpressionNode : public BinaryNode
{
public:
    EqualityExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class RelationalExpressionNode : public BinaryNode
{
public:
    RelationalExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class ShiftExpressionNode : public BinaryNode
{
public:
    ShiftExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class AdditiveExpressionNode : public BinaryNode
{
public:
    AdditiveExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class MultiplicativeExpressionNode : public BinaryNode
{
public:
    MultiplicativeExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class PMExpressionNode : public BinaryNode
{
public:
    PMExpressionNode(const Span& span_, Node* left_, Node* right_, Operator op_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class CastExpressionNode : public Node
{
public:
    CastExpressionNode(const Span& span_, Node* typeExpr_, Node* expr_);
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Expr() { return expr.get(); }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> expr;
};

class UnaryExpressionNode : public UnaryNode
{
public:
    UnaryExpressionNode(const Span& span_, Operator op_, Node* expr_);
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
private:
    Operator op;
};

class NewExpressionNode : public Node
{
public:
    NewExpressionNode(const Span& span_, Node* placement_, Node* typeExpr_, Node* initializer_);
    void Accept(Visitor& visitor) override;
    Node* Placement() { return placement.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Initializer() { return initializer.get(); }
private:
    std::unique_ptr<Node> placement;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> initializer;
};

class DeleteExpressionNode : public UnaryNode
{
public:
    DeleteExpressionNode(const Span& span_, bool array__, Node* ptr_);
    void Accept(Visitor& visitor) override;
    bool Array() const { return array_; }
private:
    bool array_;
};

class SubscriptExpressionNode : public UnaryNode
{
public:
    SubscriptExpressionNode(const Span& span_, Node* subject_, Node* index_);
    void Accept(Visitor& visitor) override;
    Node* Index() { return index.get(); }
private:
    std::unique_ptr<Node> index;
};

class InvokeExpressionNode : public UnaryNode
{
public:
    InvokeExpressionNode(const Span& span_, Node* subject_, Node* arguments_);
    void Accept(Visitor& visitor) override;
    Node* Arguments() { return arguments.get(); }
private:
    std::unique_ptr<Node> arguments;
};

class DotNode : public UnaryNode
{
public:
    DotNode(const Span& span_, Node* subject_, Node* id_);
    void Accept(Visitor& visitor) override;
    Node* Id() { return id.get(); }
private:
    std::unique_ptr<Node> id;
};

class ArrowNode : public UnaryNode
{
public:
    ArrowNode(const Span& span_, Node* subject_, Node* id_);
    void Accept(Visitor& visitor) override;
    Node* Id() { return id.get(); }
private:
    std::unique_ptr<Node> id;
};

class PostfixIncNode : public UnaryNode
{
public:
    PostfixIncNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class PostfixDecNode : public UnaryNode
{
public:
    PostfixDecNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class CppCastExpressionNode : public Node
{
public:
    CppCastExpressionNode(const Span& span_, Node* typeExpr_, Node* expr_, Operator op_);
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() { return typeExpr.get(); }
    Node* Expr() { return expr.get(); }
    Operator Op() const { return op; }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> expr;
    Operator op;
};

class SimpleTypeCastExpressionNode : public Node
{
public:
    SimpleTypeCastExpressionNode(const Span& span_, SimpleTypeNode* simpleType_, Node* expr_);
    void Accept(Visitor& visitor) override;
    SimpleTypeNode* SimpleType() { return simpleType.get(); }
    Node* Expr() { return expr.get(); }
private:
    std::unique_ptr<SimpleTypeNode> simpleType;
    std::unique_ptr<Node> expr;
};

class TypeIdExpressionNode : public UnaryNode
{
public:
    TypeIdExpressionNode(const Span& span_, Node* subject_);
    void Accept(Visitor& visitor) override;
};

class ThisNode : public Node
{
public:
    ThisNode(const Span& span_);
    void Accept(Visitor& visitor) override;
};

class IdentifierNode : public Node
{
public:
    IdentifierNode(const Span& span_, const std::u32string& identifier_);
    void Accept(Visitor& visitor) override;
    bool IsIdentifierNode() const override { return true; }
    const std::u32string& Identifier() const { return identifier; }
private:
    std::u32string identifier;
};

class OperatorFunctionIdNode : public IdentifierNode
{
public:
    OperatorFunctionIdNode(const Span& span_, Operator op_);
    bool IsOperatorFunctionIdNode() const override { return true; }
    void Accept(Visitor& visitor) override;
    Operator Op() const { return op; }
    std::u32string OpStr() const;
    std::u32string Str() const;
    std::u32string GroupName() const;
private:
    Operator op;
};

class ConversionFunctionIdNode : public IdentifierNode
{
public:
    ConversionFunctionIdNode(const Span& span_, Node* typeExpr_);
    bool IsConversionFunctionIdNode() const override { return true; }
    void Accept(Visitor& visitor) override;
    Node* TypeExpr() { return typeExpr.get(); }
private:
    std::unique_ptr<Node> typeExpr;
};

class DtorIdNode : public IdentifierNode
{
public:
    DtorIdNode(const Span& span_, const std::u32string& identifier_);
    bool IsDtorIdNode() const override { return true; }
    void Accept(Visitor& visitor) override;
};

class NestedIdNode : public BinaryNode
{
public:
    NestedIdNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class ParenthesizedExprNode : public UnaryNode
{
public:
    ParenthesizedExprNode(const Span& span_, Node* expr_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_EXPRESSION_INCLUDED
