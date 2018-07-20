// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPBINDER_STATEMENT_BINDER_INCLUDED
#define GENDOC_CPPBINDER_STATEMENT_BINDER_INCLUDED
#include <gendoc/cppbinder/BoundSourceFile.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppast/Visitor.hpp>

namespace gendoc { namespace cppbinder {

using namespace gendoc::cppsym;

class StatementBinder : public gendoc::cppast::Visitor
{
public:
    StatementBinder(SymbolTable& symbolTable_, const std::unordered_map<SourceFileNode*, BoundSourceFile*>& sourceFileMap_);
    void Visit(SourceFileNode& sourceFileNode) override;
    void Visit(SourceFileSequenceNode& sourceFileSequenceNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(DeclarationSequenceNode& declarationSequenceNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(SpecialMemberFunctionNode& specialMemberFunctionNode) override;
    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(StatementSequenceNode& statementSequenceNode) override;
    void Visit(LabeledStatementNode& labeledStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(DeclarationStatementNode& declarationStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(HandlerNode& handlerNode) override;
    void Visit(HandlerSequenceNode& handlerSequenceNode) override;
private:
    SymbolTable& symbolTable;
    const std::unordered_map<SourceFileNode*, BoundSourceFile*>& sourceFileMap;
    ContainerScope* containerScope;
    BoundSourceFile* currentSourceFile;
    FunctionSymbol* currentFunction;
};

} } // namespace gendoc::cppbinder

#endif // GENDOC_CPPBINDER_STATEMENT_BINDER_INCLUDED
