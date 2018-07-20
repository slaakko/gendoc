// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPAST_STATEMENT_INCLUDED
#define GENDOC_CPPAST_STATEMENT_INCLUDED
#include <gendoc/cppast/Node.hpp>

namespace gendoc { namespace cppast {

class LabeledStatementNode : public UnaryNode
{
public:
    LabeledStatementNode(const Span& span_, const std::u32string& label_, Node* statement_);
    void Accept(Visitor& visitor) override;
    const std::u32string& Label() const { return label; }
private:
    std::u32string label;
};

class CaseStatementNode : public UnaryNode
{
public:
    CaseStatementNode(const Span& span_, Node* caseExpr_, Node* statement_);
    void Accept(Visitor& visitor) override;
    Node* CaseExpr() { return caseExpr.get(); }
private:
    std::unique_ptr<Node> caseExpr;
};

class DefaultStatementNode : public UnaryNode
{
public:
    DefaultStatementNode(const Span& span_, Node* statement_);
    void Accept(Visitor& visitor) override;
};

class ExpressionStatementNode : public UnaryNode
{
public:
    ExpressionStatementNode(const Span& span_, Node* expression_);
    void Accept(Visitor& visitor) override;
};

class CompoundStatementNode : public UnaryNode
{
public:
    CompoundStatementNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    void AddStatement(const Span& span_, Node* statement);
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Span endBraceSpan;
};

class StatementSequenceNode : public BinaryNode
{
public:
    StatementSequenceNode(const Span& span_, Node* leftStatement_, Node* rightStatement_);
    void Accept(Visitor& visitor) override;
};

class IfStatementNode : public Node
{
public:
    IfStatementNode(const Span& span_, Node* condition_, Node* thenS_, Node* elseS_, const Span& elseSpan_);
    void Accept(Visitor& visitor) override;
    Node* Condition() { return condition.get(); }
    Node* ThenS() { return thenS.get(); }
    Node* ElseS() { return elseS.get(); }
    const Span& ElseSpan() const { return elseSpan; }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> thenS;
    std::unique_ptr<Node> elseS;
    Span elseSpan;
};

class SwitchStatementNode : public Node
{
public:
    SwitchStatementNode(const Span& span_, Node* condition_, Node* statement_);
    void Accept(Visitor& visitor) override;
    Node* Condition() { return condition.get(); }
    Node* Statement() { return statement.get(); }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> statement;
};

class WhileStatementNode : public Node
{
public:
    WhileStatementNode(const Span& span_, Node* condition_, Node* statement_);
    void Accept(Visitor& visitor) override;
    Node* Condition() { return condition.get(); }
    Node* Statement() { return statement.get(); }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> statement;
};

class DoStatementNode : public Node
{
public:
    DoStatementNode(const Span& span_, Node* statement_, Node* condition_, const Span& whileSpan_);
    void Accept(Visitor& visitor) override;
    Node* Statement() { return statement.get(); }
    Node* Condition() { return condition.get(); }
    const Span& WhileSpan() const { return whileSpan; }
private:
    std::unique_ptr<Node> statement;
    std::unique_ptr<Node> condition;
    Span whileSpan;
};

class RangeForStatementNode : public Node
{
public:
    RangeForStatementNode(const Span& span_, Node* forRangeDeclaration_, Node* forRangeInitializer_, Node* statement_);
    void Accept(Visitor& visitor) override;
    Node* ForRangeDeclaration() { return forRangeDeclaration.get(); }
    Node* ForRangeInitializer() { return forRangeInitializer.get(); }
    Node* Statement() { return statement.get(); }
private:
    std::unique_ptr<Node> forRangeDeclaration;
    std::unique_ptr<Node> forRangeInitializer;
    std::unique_ptr<Node> statement;
};

class ForStatementNode : public Node
{
public:
    ForStatementNode(const Span& span_, Node* initS_, Node* condition_, Node* loopExpr_, Node* actionS_);
    void Accept(Visitor& visitor) override;
    Node* InitS() { return initS.get(); }
    Node* Condition() { return condition.get(); }
    Node* LoopExpr() { return loopExpr.get(); }
    Node* ActionS() { return actionS.get(); }
private:
    std::unique_ptr<Node> initS;
    std::unique_ptr<Node> condition;
    std::unique_ptr<Node> loopExpr;
    std::unique_ptr<Node> actionS;
};

class BreakStatementNode : public Node
{
public:
    BreakStatementNode(const Span& span_);
    void Accept(Visitor& visitor) override;
};

class ContinueStatementNode : public Node
{
public:
    ContinueStatementNode(const Span& span_);
    void Accept(Visitor& visitor) override;
};

class ReturnStatementNode : public Node
{
public:
    ReturnStatementNode(const Span& span_, Node* returnExpr_);
    void Accept(Visitor& visitor) override;
    Node* ReturnExpr() { return returnExpr.get(); }
private:
    std::unique_ptr<Node> returnExpr;
};

class GotoStatementNode : public Node
{
public:
    GotoStatementNode(const Span& span_, const std::u32string& target_);
    void Accept(Visitor& visitor) override;
    const std::u32string& Target() const { return target; }
private:
    std::u32string target;
};

class DeclarationStatementNode : public UnaryNode
{
public:
    DeclarationStatementNode(const Span& span_, Node* declaration_);
    void Accept(Visitor& visitor) override;
};

class TryStatementNode : public Node
{
public:
    TryStatementNode(const Span& span_, CompoundStatementNode* tryBlock_);
    void Accept(Visitor& visitor) override;
    void AddHandler(const Span& span, Node* handler);
    CompoundStatementNode* TryBlock() { return tryBlock.get(); }
    Node* Handlers() { return handlers.get(); }
private:
    std::unique_ptr<CompoundStatementNode> tryBlock;
    std::unique_ptr<Node> handlers;
};

class HandlerNode : public Node
{
public:
    HandlerNode(const Span& span_, Node* exceptionDeclaration_, CompoundStatementNode* catchBlock_);
    void Accept(Visitor& visitor) override;
    Node* ExceptionDeclaration() { return exceptionDeclaration.get(); }
    CompoundStatementNode* CatchBlock() { return catchBlock.get(); }
private:
    std::unique_ptr<Node> exceptionDeclaration;
    std::unique_ptr<CompoundStatementNode> catchBlock;
};

class HandlerSequenceNode : public BinaryNode
{
public:
    HandlerSequenceNode(const Span& span_, Node* left_, Node* right_);
    void Accept(Visitor& visitor) override;
};

class CatchAllNode : public Node
{
public:
    CatchAllNode(const Span& span_);
    void Accept(Visitor& visitor) override;
};

} } // namespace gendoc::cppast

#endif // GENDOC_CPPAST_STATEMENT_INCLUDED
