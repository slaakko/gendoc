// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_HTML_SOURCE_CODE_GENERATOR_INCLUDED
#define GENDOC_HTML_SOURCE_CODE_GENERATOR_INCLUDED
#include <gendoc/gendoc/HtmlSourceCodeWriter.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppast/Visitor.hpp>
#include <gendoc/cppast/SourceFile.hpp>

namespace gendoc { namespace html {

using namespace gendoc::cppsym;

class HtmlSourceCodeGenerator : public gendoc::cppast::Visitor
{
public:
    HtmlSourceCodeGenerator(const std::vector<std::u32string>& inputLines_, int numDigits_, const std::string& styleDirName_, const std::string& styleFileName_,
        SymbolTable& symbolTable_, int inlineCodeLimit_, std::unordered_map<std::u32string, std::unique_ptr<dom::Element>>& inlineCodeMap_);
    std::u32string MakeSymbolRef(Symbol* symbol);
    std::u32string MakeInlineSymbolRef(Symbol* symbol);
    void WriteId(IdentifierNode* id, Symbol* symbol, bool writeType);
    void WriteIdSequence(bool writeType, Symbol* terminalSymbol);
    void WriteType(TypeSymbol* type, const std::vector<IdentifierNode*>& idNodeSequence);
    void WriteDocument();
    void MoveTo(const Span& span);
    void OpenLine();
    void OpenInlineLine();
    void CloseLine();
    void UseInputLine();
    void WriteRestOfInput();
    void WriteSpecifiers(Specifier specifiers);
    void WriteAccess(const Span& span, Specifier access);
    void Visit(SourceFileNode& sourceFileNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(SimpleDeclarationNode& simpleDeclarationNode) override;
    void Visit(AliasDeclarationNode& aliasDeclarationNode) override;
    void Visit(DeclarationSequenceNode& declarationSequenceNode) override;
    void Visit(ForwardClassDeclarationNode& forwardClassDeclarationNode) override;
    void Visit(ElaborateClassNameNode& elaborateClassNameNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(BaseClassSpecifierNode& baseClassSpecifierNode) override;
    void Visit(BaseClassSpecifierSequenceNode& baseClassSpecifierSequenceNode) override;
    void Visit(MemberAccessDeclarationNode& memberAccessDeclarationNode) override;
    void Visit(MemberDeclarationNode& memberDeclarationNode) override;
    void Visit(FloatingLiteralNode& floatingLiteralNode) override;
    void Visit(IntegerLiteralNode& integerLiteralNode) override;
    void Visit(CharacterLiteralNode& characterLiteralNode) override;
    void Visit(StringLiteralNode& stringLiteralNode) override;
    void Visit(BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(NullPtrLiteralNode& nullPtrLiteralNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(SpecialMemberFunctionNode& specialMemberFunctionNode) override;
    void Visit(LabeledStatementNode& labeledStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(StatementSequenceNode& statementSequenceNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(BreakStatementNode& breakStatementNode) override;
    void Visit(ContinueStatementNode& continueStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(GotoStatementNode& gotoStatementNode) override;
    void Visit(DeclarationStatementNode& declarationStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(HandlerNode& handlerNode) override;
    void Visit(HandlerSequenceNode& handlerSequenceNode) override;
    void Visit(CatchAllNode& catchAllNode) override;
    void Visit(CtorInitializerNode& ctorInitializerNode) override;
    void Visit(ExpressionListInitializerNode& expressionListInitializer) override;
    void Visit(AssignmentInitializerNode& assignmentInitializerNode) override;
    void Visit(ExpressionInitializerNode& expressionInitializerNode) override;
    void Visit(EnumTypeNode& enumTypeNode) override;
    void Visit(EnumeratorNode& enumeratorNode) override;
    void Visit(EnumeratorSequenceNode& enumeratorSequenceNode) override;
    void Visit(ExpressionSequenceNode& expressionSequenceNode) override;
    void Visit(CommaExpressionNode& commaExpressionNode) override;
    void Visit(AssignmentExpressionNode& assignmentExpressionNode) override;
    void Visit(ConditionalExpressionNode& conditionalExpressionNode) override;
    void Visit(ThrowExpressionNode& throwExpressionNode) override;
    void Visit(LogicalOrExpressionNode& logicalOrExpressionNode) override;
    void Visit(LogicalAndExpressionNode& logicalAndExpressionNode) override;
    void Visit(InclusiveOrExpressionNode& inclusiveOrExpressionNode) override;
    void Visit(ExclusiveOrExpressionNode& exclusiveOrExpressionNode) override;
    void Visit(AndExpressionNode& andExpressionNode) override;
    void Visit(EqualityExpressionNode& equalityExpressionNode) override;
    void Visit(RelationalExpressionNode& relationalExpressionNode) override;
    void Visit(ShiftExpressionNode& shiftExpressionNode) override;
    void Visit(AdditiveExpressionNode& additiveExpressionNode) override;
    void Visit(MultiplicativeExpressionNode& multiplicativeExpressionNode) override;
    void Visit(PMExpressionNode& pmExpressionNode) override;
    void Visit(CastExpressionNode& castExpressionNode) override;
    void Visit(UnaryExpressionNode& unaryExpressionNode) override;
    void Visit(NewExpressionNode& newExpressionNode) override;
    void Visit(DeleteExpressionNode& deleteExpressionNode) override;
    void Visit(SubscriptExpressionNode& subscriptExpressionNode) override;
    void Visit(InvokeExpressionNode& invokeExpressionNode) override;
    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void Visit(PostfixIncNode& postfixIncNode) override;
    void Visit(PostfixDecNode& postfixDecNode) override;
    void Visit(CppCastExpressionNode& cppCastExpressionNode) override;
    void Visit(SimpleTypeCastExpressionNode& simpleTypeCastExpressionNode) override;
    void Visit(TypeIdExpressionNode& typeIdExpressionNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(ParenthesizedExprNode& parenthesizedExprNode) override;
    void Visit(BracedInitializerListNode& bracedInitializerListNode) override;
    void Visit(MemberInitializerNode& memberInitializerNode) override;
    void Visit(MemberInitializerSequenceNode& memberInitializerSequenceNode) override;
    void Visit(FunctionDeclaratorNode& functionDeclaratorNode) override;
    void Visit(FunctionPtrIdNode& functionPtrIdNode) override;
    void Visit(MemberFunctionPtrIdNode& memberFunctionPtrIdNode) override;
    void Visit(ArrayDeclaratorNode& arrayDeclaratorNode) override;
    void Visit(InitDeclaratorNode& initDeclaratorNode) override;
    void Visit(IdDeclaratorNode& idDeclaratorNode) override;
    void Visit(ParameterNode& parameterNode) override;
    void Visit(ParameterSequenceNode& parameterSequenceNode) override;
    void Visit(TypeExprNode& typeExprNode) override;
    void Visit(SimpleTypeNode& simpleTypeNode) override;
    void Visit(TypeParameterNode& typeParameterNode) override;
    void Visit(TemplateParameterSequenceNode& templateParameterSequenceNode) override;
    void Visit(TemplateDeclarationNode& templateDeclarationNode) override;
    void Visit(TemplateArgumentSequenceNode& templateArgumentSequenceNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(TemplateArgumentNode& templateArgumentNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(NestedIdNode& nestedIdNode) override;
    void Visit(OperatorFunctionIdNode& operatorFunctionIdNode) override;
    void Visit(ConversionFunctionIdNode& conversionFunctionIdNode) override;
    void Visit(DtorIdNode& dtorIdNode) override;
private:
    SymbolTable& symbolTable;
    FunctionSymbol* currentFunction;
    Symbol* invokeSymbol;
    HtmlSourceCodeWriter writer;
    const std::vector<std::u32string>& inputLines;
    int numDigits;
    std::string styleFilePath;
    std::string contentFilePath;
    int currentSourceLineNumber;
    std::string htmlFilePath;
    dom::Document htmlSourceFileDocument;
    std::unique_ptr<dom::Element> htmlElement;
    std::unique_ptr<dom::Element> bodyElement;
    std::unique_ptr<dom::Element> lineElement;
    std::unique_ptr<dom::Element> inlineLineElement;
    std::unique_ptr<dom::Element> inlineCodeElement;
    std::vector<std::pair<IdentifierNode*, Symbol*>> idSequence;
    std::vector<int>* lineStarts;
    bool firstBaseClass;
    bool firstParam;
    bool firstTemplateParameter;
    bool firstTemplateArgument;
    bool firstMemberInitializer;
    bool firstInitializer;
    bool firstEnumerator;
    bool rangeForDeclaration;
    int blockLevel;
    Specifier currentAccess;
    int inlineCodeLimit;
    int inlineCodeOffset;
    bool skip;
    std::unordered_map<std::u32string, std::unique_ptr<dom::Element>>& inlineCodeMap;
};

} } // namespace gendoc::html

#endif // GENDOC_HTML_SOURCE_CODE_GENERATOR_INCLUDED
