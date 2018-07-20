// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/Content.hpp>
#include <gendoc/cppbinder/TypeBinder.hpp>
#include <gendoc/cppbinder/VirtualBinder.hpp>
#include <gendoc/cppbinder/StatementBinder.hpp>
#include <gendoc/cppsym/SymbolTable.hpp>
#include <gendoc/cppsym/SymbolCreator.hpp>
#include <gendoc/cppparser/IncludeDirectiveGrammar.hpp>
#include <gendoc/cppparser/CppSourceFileGrammar.hpp>
#include <gendoc/syntax/ParserFile.hpp>
#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Rule.hpp>
#include <gendoc/parsing/Scope.hpp>
#include <gendoc/parsing/Nonterminal.hpp>
#include <gendoc/parsing/Visitor.hpp>
#include <gendoc/parsing/Primitive.hpp>
#include <gendoc/parsing/Composite.hpp>
#include <gendoc/cppast/Visitor.hpp>
#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/dom/Parser.hpp>
#include <gendoc/dom/CharacterData.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/util/MappedInputFile.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/util/Util.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace gendoc { namespace content {

using namespace gendoc::cppbinder;
using namespace gendoc::cppsym;
using namespace gendoc::cppparser;
using namespace gendoc::cppast;
using namespace gendoc::syntax;
using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

class IncludeDirectiveResolver : public gendoc::cppast::Visitor
{
public:
    IncludeDirectiveResolver(SourceFileNode* sourceFileNode_, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles_);
    void Visit(SourceFileNode& sourceFileNode) override;
    void Visit(IncludeDirectiveNode& includeDirectiveNode) override;
    void Visit(IncludeDirectiveSequenceNode& includeDirectiveSequenceNode) override;
private:
    SourceFileNode * sourceFileNode;
    const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles;
};

IncludeDirectiveResolver::IncludeDirectiveResolver(SourceFileNode* sourceFileNode_, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles_) :
    sourceFileNode(sourceFileNode_), allSourceFiles(allSourceFiles_)
{
}

void IncludeDirectiveResolver::Visit(SourceFileNode& sourceFileNode)
{
    if (sourceFileNode.IncludeDirectives())
    {
        sourceFileNode.IncludeDirectives()->Accept(*this);
    }
}

void IncludeDirectiveResolver::Visit(IncludeDirectiveNode& includeDirectiveNode)
{
    for (const std::pair<std::string, SourceFileNode*>& sourceFilePathNode : allSourceFiles)
    {
        if (LastComponentsEqual(includeDirectiveNode.IncludeFilePath(), sourceFilePathNode.first, '/'))
        {
            includeDirectiveNode.SetSourceFileNode(sourceFilePathNode.second);
            break;
        }
    }
}

void IncludeDirectiveResolver::Visit(IncludeDirectiveSequenceNode& includeDirectiveSequenceNode)
{
    includeDirectiveSequenceNode.Left()->Accept(*this);
    includeDirectiveSequenceNode.Right()->Accept(*this);
}

void ResolveIncludeDirectives(SourceFileNode* sourceFileNode, const std::unordered_map<std::string, SourceFileNode*>& allSourceFiles)
{
    IncludeDirectiveResolver resolver(sourceFileNode, allSourceFiles);
    sourceFileNode->Accept(resolver);
}

CppSourceFile* cppSourceFileGrammar = nullptr;
IncludeDirective* includeDirectiveGrammar = nullptr;

std::unique_ptr<SourceFileNode> ParseSourceFile(const std::string& sourceFilePath, const std::string& relativeSourceFilePath, const std::u32string& projectName, 
    bool debugParse, int& sourceFileIndex)
{
    if (!cppSourceFileGrammar)
    {
        cppSourceFileGrammar = CppSourceFile::Create();
    }
    if (!includeDirectiveGrammar)
    {
        includeDirectiveGrammar = IncludeDirective::Create();
    }
    if (debugParse)
    {
        cppSourceFileGrammar->SetLog(&std::cout);
        cppSourceFileGrammar->SetMaxLogLineLength(1024);
    }
    std::u32string sourceFileContent;
    if (boost::filesystem::file_size(sourceFilePath) > 0)
    {
        MappedInputFile sourceFile(sourceFilePath);
        sourceFileContent = ToUtf32(std::string(sourceFile.Begin(), sourceFile.End()));
    }
    std::unique_ptr<SourceFileNode> sourceFileNode(new SourceFileNode(Span(), sourceFilePath, relativeSourceFilePath, projectName));
    includeDirectiveGrammar->Parse(sourceFileContent.c_str(), sourceFileContent.c_str() + sourceFileContent.length(), sourceFileIndex, sourceFilePath, sourceFileNode.get());
    cppSourceFileGrammar->Parse(sourceFileContent.c_str(), sourceFileContent.c_str() + sourceFileContent.length(), sourceFileIndex, sourceFilePath, sourceFileNode.get());
    sourceFileNode->SetSourceFileIndex(sourceFileIndex);
    sourceFileNode->ComputeLineStarts(sourceFileContent);
    ++sourceFileIndex;
    return sourceFileNode;
}

std::unique_ptr<gendoc::cppast::Node> ParseSourceFiles(const std::vector<std::pair<std::string, std::string>>& sourceFilePaths, 
    std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles, std::unordered_map<std::string, SourceFileNode*>& allSourceFiles, 
    bool verbose, int& sourceFileIndex, const std::u32string& projectName)
{
    if (verbose)
    {
        std::cout << "parsing sources..." << std::endl;
    }
    std::unique_ptr<gendoc::cppast::Node> node;
    std::vector<SourceFileNode*> sourceFileNodes;
    for (const std::pair<std::string, std::string>& sourceFilePath : sourceFilePaths)
    {
        if (verbose)
        {
            std::cout << "> " << sourceFilePath.first << std::endl;
        }
        if (sourceFileIndex != sourceFiles.size())
        {
            throw std::runtime_error("source file index does not match");
        }
        std::unique_ptr<SourceFileNode> sourceFileNode = ParseSourceFile(sourceFilePath.first, sourceFilePath.second, projectName, false, sourceFileIndex);
        sourceFiles.push_back(sourceFileNode.get());
        sourceFileNodes.push_back(sourceFileNode.get());
        if (node)
        {
            node.reset(new SourceFileSequenceNode(Span(), node.release(), sourceFileNode.release()));
        }
        else
        {
            node.reset(sourceFileNode.release());
        }
    }
    for (SourceFileNode* sourceFileNode : sourceFileNodes)
    {
        allSourceFiles[sourceFileNode->SourceFilePath()] = sourceFileNode;
    }
    for (SourceFileNode* sourceFileNode : sourceFileNodes)
    {
        ResolveIncludeDirectives(sourceFileNode, allSourceFiles);
    }
    return node;
}

ParserFile* parserFileGrammar = nullptr;

void ParseParserSourceFiles(const std::vector<std::pair<std::string, std::string>>& parserSourceFilePaths, ParsingDomain* parsingDomain,
    std::vector<std::unique_ptr<ParserFileContent>>& parserFileContents, int& parserSourceFileIndex, int projectId, const std::u32string& projectName,
    std::unordered_map<int, std::u32string>& parserProjectMap, bool verbose)
{
    parserProjectMap[projectId] = projectName;
    if (!parserFileGrammar)
    {
        parserFileGrammar = ParserFile::Create();
    }
    if (verbose)
    {
        std::cout << "parsing parser sources..." << std::endl;
    }
    for (const std::pair<std::string, std::string>& p : parserSourceFilePaths)
    {
        const std::string& sourceFilePath = p.first;
        const std::string& sourceFileName = p.second;
        if (verbose)
        {
            std::cout << "> " << sourceFilePath << std::endl;
        }
        MappedInputFile parserSourceFile(sourceFilePath);
        std::u32string parserSourceFileContent = ToUtf32(std::string(parserSourceFile.Begin(), parserSourceFile.End()));
        std::unique_ptr<ParserFileContent> parserFileContent(parserFileGrammar->Parse(
            parserSourceFileContent.c_str(), parserSourceFileContent.c_str() + parserSourceFileContent.length(), parserSourceFileIndex, sourceFilePath, projectId, parsingDomain));
        parserFileContent->SetRelativeFilePath(sourceFileName);
        ++parserSourceFileIndex;
        parserFileContents.push_back(std::move(parserFileContent));
    }
}

class ParserLinkerVisitor : public gendoc::parsing::Visitor
{
public:
    ParserLinkerVisitor(int projectId_);
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void BeginVisit(Rule& rule) override;
    void EndVisit(Rule& rule) override;
    void Visit(RuleLink& link) override;
    void Visit(NonterminalParser& parser) override;
private:
    int projectId;
    Grammar* currentGrammar;
    Rule* currentRule;
};

ParserLinkerVisitor::ParserLinkerVisitor(int projectId_) : projectId(projectId_), currentGrammar(nullptr), currentRule(nullptr)
{
}

void ParserLinkerVisitor::BeginVisit(Grammar& grammar)
{
    if (grammar.Owner() == projectId)
    {
        currentGrammar = &grammar;
    }
    else
    {
        currentGrammar = nullptr;
    }
}

void ParserLinkerVisitor::EndVisit(Grammar& grammar)
{
    currentGrammar = nullptr;
}

void ParserLinkerVisitor::BeginVisit(Rule& rule)
{
    if (currentGrammar)
    {
        currentRule = &rule;
    }
    else
    {
        currentRule = nullptr;
    }
}

void ParserLinkerVisitor::EndVisit(Rule& rule)
{
    currentRule = nullptr;
}

void ParserLinkerVisitor::Visit(RuleLink& link)
{
    if (currentGrammar)
    {
        ParsingObject* object = currentGrammar->GetScope()->Get(link.LinkedRuleName(), ObjectKind::rule); 
        if (object)
        {
            if (object->IsRule())
            {
                Rule* rule = static_cast<Rule*>(object);
                link.SetRule(rule);
                link.AddToScope();
            }
        }
    }
}

void ParserLinkerVisitor::Visit(NonterminalParser& parser)
{
    if (currentGrammar)
    {
        Rule* rule = currentGrammar->GetRule(parser.RuleName()); 
        if (rule)
        {
            parser.SetRule(rule);
        }
        else
        {
            throw std::runtime_error("rule not found");
        }
    }
}

class ParserSymbolCreatorVisitor : public gendoc::parsing::Visitor
{
public:
    ParserSymbolCreatorVisitor(SymbolTable& symbolTable_, int projectId_, const std::unordered_map<int, std::u32string>& parserProjectMap_, 
        std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap_);
    void BeginVisit(Namespace& ns) override;
    void EndVisit(Namespace& ns) override;
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void BeginVisit(Rule& rule) override;
    void EndVisit(Rule& rule) override;
private:
    SymbolTable& symbolTable;
    const std::unordered_map<int, std::u32string>& parserProjectMap;
    std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap;
    int projectId;
    std::u32string projectName;
    Grammar* currentGrammar;
    RuleSymbol* currentRuleSymbol;
};

ParserSymbolCreatorVisitor::ParserSymbolCreatorVisitor(SymbolTable& symbolTable_, int projectId_, 
    const std::unordered_map<int, std::u32string>& parserProjectMap_, std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap_) :
    symbolTable(symbolTable_), projectId(projectId_), parserProjectMap(parserProjectMap_), ruleSymbolMap(ruleSymbolMap_), currentGrammar(nullptr), currentRuleSymbol(nullptr)
{
    auto it = parserProjectMap.find(projectId);
    if (it != parserProjectMap.cend())
    {
        projectName = it->second;
    }
}

void ParserSymbolCreatorVisitor::BeginVisit(Namespace& ns)
{
    symbolTable.BeginNamespace(ns.Name(), std::u32string());
}

void ParserSymbolCreatorVisitor::EndVisit(Namespace& ns)
{
    symbolTable.EndNamespace();
}

void ParserSymbolCreatorVisitor::BeginVisit(Grammar& grammar)
{
    if (grammar.Owner() == projectId)
    {
        symbolTable.BeginGrammar(grammar.GetSpan(), grammar.Name());
        currentGrammar = &grammar;
    }
    else
    {
        currentGrammar = nullptr;
    }
}

void ParserSymbolCreatorVisitor::EndVisit(Grammar& grammar)
{
    if (grammar.Owner() == projectId)
    {
        symbolTable.EndGrammar(projectName);
    }
    currentGrammar = nullptr;
}

void ParserSymbolCreatorVisitor::BeginVisit(Rule& rule)
{
    if (currentGrammar)
    {
        currentRuleSymbol = symbolTable.AddRule(rule.GetSpan(), rule.Name());
        ruleSymbolMap[&rule] = currentRuleSymbol;
    }
    else
    {
        currentRuleSymbol = nullptr;
    }
}

void ParserSymbolCreatorVisitor::EndVisit(Rule& rule)
{
    currentRuleSymbol = nullptr;
}

class RuleBodyCreatorVisitor : public gendoc::parsing::Visitor
{
public:
    RuleBodyCreatorVisitor(SymbolTable& symbolTable_, int projectId_, const std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap_);
    RuleSymbol* GetRuleSymbol(Rule* rule);
    void BeginVisit(Namespace& ns) override;
    void EndVisit(Namespace& ns) override;
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void BeginVisit(Rule& rule) override;
    void EndVisit(Rule& rule) override;
    void Visit(CharParser& parser) override;
    void Visit(StringParser& parser) override;
    void Visit(CharSetParser& parser) override;
    void Visit(EmptyParser& parser) override;
    void Visit(SpaceParser& parser) override;
    void Visit(LetterParser& parser) override;
    void Visit(UpperLetterParser& parser) override;
    void Visit(LowerLetterParser& parser) override;
    void Visit(TitleLetterParser& parser) override;
    void Visit(ModifierLetterParser& parser) override;
    void Visit(OtherLetterParser& parser) override;
    void Visit(CasedLetterParser& parser) override;
    void Visit(DigitParser& parser) override;
    void Visit(HexDigitParser& parser) override;
    void Visit(MarkParser& parser) override;
    void Visit(NonspacingMarkParser& parser) override;
    void Visit(SpacingMarkParser& parser) override;
    void Visit(EnclosingMarkParser& parser) override;
    void Visit(NumberParser& parser) override;
    void Visit(DecimalNumberParser& parser) override;
    void Visit(LetterNumberParser& parser) override;
    void Visit(OtherNumberParser& parser) override;
    void Visit(PunctuationParser& parser) override;
    void Visit(ConnectorPunctuationParser& parser) override;
    void Visit(DashPunctuationParser& parser) override;
    void Visit(OpenPunctuationParser& parser) override;
    void Visit(ClosePunctuationParser& parser) override;
    void Visit(InitialPunctuationParser& parser) override;
    void Visit(FinalPunctuationParser& parser) override;
    void Visit(OtherPunctuationParser& parser) override;
    void Visit(SymbolParser& parser) override;
    void Visit(MathSymbolParser& parser) override;
    void Visit(CurrencySymbolParser& parser) override;
    void Visit(ModifierSymbolParser& parser) override;
    void Visit(OtherSymbolParser& parser) override;
    void Visit(SeparatorParser& parser) override;
    void Visit(SpaceSeparatorParser& parser) override;
    void Visit(LineSeparatorParser& parser) override;
    void Visit(ParagraphSeparatorParser& parser) override;
    void Visit(OtherParser& parser) override;
    void Visit(ControlParser& parser) override;
    void Visit(FormatParser& parser) override;
    void Visit(SurrogateParser& parser) override;
    void Visit(PrivateUseParser& parser) override;
    void Visit(UnassignedParser& parser) override;
    void Visit(GraphicParser& parser) override;
    void Visit(BaseCharParser& parser) override;
    void Visit(AlphabeticParser& parser) override;
    void Visit(IdStartParser& parser) override;
    void Visit(IdContParser& parser) override;
    void Visit(AnyCharParser& parser) override;
    void Visit(RangeParser& parser) override;
    void BeginVisit(OptionalParser& parser) override;
    void EndVisit(OptionalParser& parser) override;
    void BeginVisit(PositiveParser& parser) override;
    void EndVisit(PositiveParser& parser) override;
    void BeginVisit(KleeneStarParser& parser) override;
    void EndVisit(KleeneStarParser& parser) override;
    void BeginVisit(ActionParser& parser) override;
    void EndVisit(ActionParser& parser) override;
    void BeginVisit(ExpectationParser& parser) override;
    void EndVisit(ExpectationParser& parser) override;
    void BeginVisit(TokenParser& parser) override;
    void EndVisit(TokenParser& parser) override;
    void BeginVisit(GroupingParser& parser) override;
    void EndVisit(GroupingParser& parser) override;
    void BeginVisit(SequenceParser& parser) override;
    void Visit(SequenceParser& parser) override;
    void EndVisit(SequenceParser& parser) override;
    void BeginVisit(AlternativeParser& parser) override;
    void Visit(AlternativeParser& parser) override;
    void EndVisit(AlternativeParser& parser) override;
    void BeginVisit(DifferenceParser& parser) override;
    void Visit(DifferenceParser& parser) override;
    void EndVisit(DifferenceParser& parser) override;
    void BeginVisit(ExclusiveOrParser& parser) override;
    void Visit(ExclusiveOrParser& parser) override;
    void EndVisit(ExclusiveOrParser& parser) override;
    void BeginVisit(IntersectionParser& parser) override;
    void Visit(IntersectionParser& parser) override;
    void EndVisit(IntersectionParser& parser) override;
    void BeginVisit(ListParser& parser) override;
    void Visit(ListParser& parser) override;
    void EndVisit(ListParser& parser) override;
    void Visit(NonterminalParser& parser) override;
    void Visit(KeywordParser& parser) override;
    void Visit(KeywordListParser& parser) override;
    void Visit(UsingObject& object) override;
    void Visit(RuleLink& link) override;
private:
    SymbolTable& symbolTable;
    int projectId;
    const std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap;
    Grammar* currentGrammar;
    RuleSymbol* currentRuleSymbol;
    std::stack<RuleSymbol*> currentRuleStack;
    bool skipping;
    std::stack<bool> skippingStack;
    int suppressGrouping;
    std::stack<int> suppressGroupingStack;
};

RuleBodyCreatorVisitor::RuleBodyCreatorVisitor(SymbolTable& symbolTable_, int projectId_, const std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap_) :
    symbolTable(symbolTable_), projectId(projectId_), ruleSymbolMap(ruleSymbolMap_), currentGrammar(nullptr), currentRuleSymbol(nullptr), skipping(false),
    suppressGrouping(-1)
{
}

RuleSymbol* RuleBodyCreatorVisitor::GetRuleSymbol(Rule* rule)
{
    auto it = ruleSymbolMap.find(rule);
    if (it != ruleSymbolMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("rule not found");
    }
}

void RuleBodyCreatorVisitor::BeginVisit(Namespace& ns)
{
}

void RuleBodyCreatorVisitor::EndVisit(Namespace& ns)
{
}

void RuleBodyCreatorVisitor::BeginVisit(Grammar& grammar)
{
    if (grammar.Owner() == projectId)
    {
        currentGrammar = &grammar;
    }
    else
    {
        currentGrammar = nullptr;
    }
}

void RuleBodyCreatorVisitor::EndVisit(Grammar& grammar)
{
    currentGrammar = nullptr;
}

void RuleBodyCreatorVisitor::BeginVisit(Rule& rule)
{
    suppressGrouping = -1;
    if (currentGrammar)
    {
        currentRuleSymbol = GetRuleSymbol(&rule);
        if (currentRuleSymbol->Name() == U"BitOr")
        {
            int x = 0;
        }
        if (rule.Definition()->IsGroupingWithAction())
        {
            suppressGrouping = 0;
        }
    }
    else
    {
        currentRuleSymbol = nullptr;
    }
}

void RuleBodyCreatorVisitor::EndVisit(Rule& rule)
{
    currentRuleSymbol = nullptr;
}

void RuleBodyCreatorVisitor::Visit(CharParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> charElement(new dom::Element(U"span"));
        charElement->SetAttribute(U"class", U"ruleChar");
        charElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"'" + CharStr(parser.GetChar()) + U"'")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(charElement.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(StringParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> stringElement(new dom::Element(U"span"));
        stringElement->SetAttribute(U"class", U"ruleString");
        stringElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"\"" + StringStr(parser.GetString()) + U"\"")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(stringElement.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(CharSetParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> charSetElement(new dom::Element(U"span"));
        charSetElement->SetAttribute(U"class", U"ruleCharSet");
        std::u32string inverse;
        if (parser.Inverse())
        {
            inverse = U"^";
        }
        charSetElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"[" + inverse + StringStr(parser.Set()) + U"]")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(charSetElement.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(EmptyParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"empty")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SpaceParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"space")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(LetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(UpperLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"upper_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(LowerLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"lower_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(TitleLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"title_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ModifierLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"modifier_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OtherLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"other_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(CasedLetterParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"cased_letter")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(DigitParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"digit")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(HexDigitParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"hexdigit")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(MarkParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"mark")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(NonspacingMarkParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"nonspacing_mark")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SpacingMarkParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"spacing_mark")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(EnclosingMarkParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"enclosing_mark")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(NumberParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"number")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(DecimalNumberParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"decimal_number")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(LetterNumberParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"letter_number")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OtherNumberParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"other_number")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(PunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ConnectorPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"connector_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(DashPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"dash_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OpenPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"open_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ClosePunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"close_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(InitialPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"initial_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(FinalPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"final_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OtherPunctuationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"other_punctuation")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SymbolParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"symbol")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(MathSymbolParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"math_symbol")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(CurrencySymbolParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"currency_symbol")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ModifierSymbolParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"modifier_symbol")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OtherSymbolParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"other_symbol")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SeparatorParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"separator")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SpaceSeparatorParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"space_separator")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(LineSeparatorParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"line_separator")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ParagraphSeparatorParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"paragraph_separator")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(OtherParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"other")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(ControlParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"control")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(FormatParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"format")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(SurrogateParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"surrogate")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(PrivateUseParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"private_use")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(UnassignedParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"unassigned")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(GraphicParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"graphic")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(BaseCharParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"basechar")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(AlphabeticParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"alphabetic")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(IdStartParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"idstart")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(IdContParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"idcont")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(AnyCharParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"anychar")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(RangeParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"range")));
        std::unique_ptr<dom::Element> params(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleParams");
        params->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(" + ToUtf32(std::to_string(parser.Start())) + U"," + ToUtf32(std::to_string(parser.End())) + U")")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(params.release()));
    }
}

void RuleBodyCreatorVisitor::BeginVisit(OptionalParser& parser)
{
    if (currentRuleSymbol)
    {
        suppressGroupingStack.push(suppressGrouping);
        if (parser.Child()->IsGroupingWithAction())
        {
            suppressGrouping = 0;
        }
        else
        {
            suppressGrouping = -1;
        }
    }
}

void RuleBodyCreatorVisitor::EndVisit(OptionalParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"?")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        suppressGrouping = suppressGroupingStack.top();
        suppressGroupingStack.pop();
    }
}

void RuleBodyCreatorVisitor::BeginVisit(PositiveParser& parser)
{
    if (currentRuleSymbol)
    {
        suppressGroupingStack.push(suppressGrouping);
        if (parser.Child()->IsGroupingWithAction())
        {
            suppressGrouping = 0;
        }
        else
        {
            suppressGrouping = -1;
        }
    }
}

void RuleBodyCreatorVisitor::EndVisit(PositiveParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"+")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        suppressGrouping = suppressGroupingStack.top();
        suppressGroupingStack.pop();
    }
}

void RuleBodyCreatorVisitor::BeginVisit(KleeneStarParser& parser)
{
    if (currentRuleSymbol)
    {
        suppressGroupingStack.push(suppressGrouping);
        if (parser.Child()->IsGroupingWithAction())
        {
            suppressGrouping = 0;
        }
        else
        {
            suppressGrouping = -1;
        }
    }
}

void RuleBodyCreatorVisitor::EndVisit(KleeneStarParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"*")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        suppressGrouping = suppressGroupingStack.top();
        suppressGroupingStack.pop();
    }
}

void RuleBodyCreatorVisitor::BeginVisit(ActionParser& parser)
{
}

void RuleBodyCreatorVisitor::EndVisit(ActionParser& parser)
{
}

void RuleBodyCreatorVisitor::BeginVisit(ExpectationParser& parser)
{
}

void RuleBodyCreatorVisitor::EndVisit(ExpectationParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"!")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::BeginVisit(TokenParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"token")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        std::unique_ptr<dom::Element> paren(new dom::Element(U"span"));
        paren->SetAttribute(U"class", U"ruleOp");
        paren->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(paren.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(TokenParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> paren(new dom::Element(U"span"));
        paren->SetAttribute(U"class", U"ruleOp");
        paren->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(paren.release()));
    }
}

void RuleBodyCreatorVisitor::BeginVisit(GroupingParser& parser)
{
    if (currentRuleSymbol)
    {
        if (suppressGrouping == -1 || suppressGrouping > 0)
        {
            std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
            element->SetAttribute(U"class", U"ruleOp");
            element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
            currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
            if (suppressGrouping > 0)
            {
                ++suppressGrouping;
            }
        }
        else
        {
            ++suppressGrouping;
        }
    }
}

void RuleBodyCreatorVisitor::EndVisit(GroupingParser& parser)
{
    if (currentRuleSymbol)
    {
        if (suppressGrouping >= 0)
        {
            --suppressGrouping;
        }
        if (suppressGrouping == -1 || suppressGrouping > 0)
        {
            std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
            element->SetAttribute(U"class", U"ruleOp");
            element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
            currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
            if (suppressGrouping > 0)
            {
                --suppressGrouping;
            }
        }
        else if (suppressGrouping == 0)
        {
            --suppressGrouping;
        }
    }
}

void RuleBodyCreatorVisitor::BeginVisit(SequenceParser& parser)
{
    if (parser.Left()->IsEmptyWithAction())
    {
        skippingStack.push(skipping);
        skipping = true;
        currentRuleStack.push(currentRuleSymbol);
        currentRuleSymbol = nullptr;
    }
}

void RuleBodyCreatorVisitor::Visit(SequenceParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
    if (skipping)
    {
        skipping = skippingStack.top();
        skippingStack.pop();
        currentRuleSymbol = currentRuleStack.top();
        currentRuleStack.pop();
    }
    if (parser.Right()->IsEmptyWithAction())
    {
        skippingStack.push(skipping);
        skipping = true;
        currentRuleStack.push(currentRuleSymbol);
        currentRuleSymbol = nullptr;
    }
}

void RuleBodyCreatorVisitor::EndVisit(SequenceParser& parser)
{
}

void RuleBodyCreatorVisitor::BeginVisit(AlternativeParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(AlternativeParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" | ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(AlternativeParser& parser)
{
}

void RuleBodyCreatorVisitor::BeginVisit(DifferenceParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(DifferenceParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"minus")));
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(DifferenceParser& parser)
{
}

void RuleBodyCreatorVisitor::BeginVisit(ExclusiveOrParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(ExclusiveOrParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ^ ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(ExclusiveOrParser& parser)
{
}

void RuleBodyCreatorVisitor::BeginVisit(IntersectionParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(IntersectionParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" & ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(IntersectionParser& parser)
{

}

void RuleBodyCreatorVisitor::BeginVisit(ListParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(ListParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleOp");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" % ")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::EndVisit(ListParser& parser)
{
}

void RuleBodyCreatorVisitor::Visit(NonterminalParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleLink");
        std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
        linkElement->SetAttribute(U"class", U"ruleLink");
        Rule* rule = parser.GetRule();
        if (!rule)
        {
            throw std::runtime_error("rule not found");
        }
        if (rule->GetGrammar() == currentGrammar)
        {
            RuleSymbol* ruleSymbol = GetRuleSymbol(rule);
            linkElement->SetAttribute(U"href", U"#" + ruleSymbol->Id());
        }
        else
        {
            RuleSymbol* ruleSymbol = GetRuleSymbol(rule);
            ContainerSymbol* parent = ruleSymbol->Parent();
            while (parent && !parent->IsGrammarSymbol())
            {
                parent = parent->Parent();
            }
            if (parent)
            {
                linkElement->SetAttribute(U"href", parent->Id() + U".html#" + ruleSymbol->Id());
            }
            else
            {
                throw std::runtime_error("grammar symbol not found");
            }
        }
        linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(parser.RuleName())));
        element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(KeywordParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"keyword")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        std::unique_ptr<dom::Element> kw(new dom::Element(U"span"));
        kw->SetAttribute(U"class", U"ruleString");
        kw->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(\"" + StringStr(parser.Keyword()) + U"\")")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(kw.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(KeywordListParser& parser)
{
    if (currentRuleSymbol)
    {
        std::unique_ptr<dom::Element> element(new dom::Element(U"span"));
        element->SetAttribute(U"class", U"ruleKeyword");
        element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"keyword_list")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        std::unique_ptr<dom::Element> preamble(new dom::Element(U"span"));
        preamble->SetAttribute(U"class", U"ruleParams");
        preamble->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(" + parser.SelectorRuleName() + U", [")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(preamble.release()));
        std::unique_ptr<dom::Element> kw(new dom::Element(U"span"));
        kw->SetAttribute(U"class", U"ruleString");
        bool first = true;
        for (const std::u32string& keyword : parser.Keywords())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                kw->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
            }
            kw->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"\"" + StringStr(keyword) + U"\"")));
        }
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(kw.release()));
        std::unique_ptr<dom::Element> postAmble(new dom::Element(U"span"));
        postAmble->SetAttribute(U"class", U"ruleParams");
        postAmble->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"])")));
        currentRuleSymbol->Body()->AppendChild(std::unique_ptr<dom::Node>(postAmble.release()));
    }
}

void RuleBodyCreatorVisitor::Visit(UsingObject& object)
{
}

void RuleBodyCreatorVisitor::Visit(RuleLink& link)
{
}

void ProcessGrammars(gendoc::cppsym::SymbolTable& symbolTable, ParsingDomain* parsingDomain, int projectId, const std::u32string& projectName,
    const std::unordered_map<int, std::u32string>& parserProjectMap, std::unordered_map<Rule*, RuleSymbol*>& ruleSymbolMap)
{
    ParserLinkerVisitor linkerVisitor(projectId);
    parsingDomain->GlobalNamespace()->Accept(linkerVisitor);
    ParserSymbolCreatorVisitor symbolCreatorVisitor(symbolTable, projectId, parserProjectMap, ruleSymbolMap);
    parsingDomain->GlobalNamespace()->Accept(symbolCreatorVisitor);
    RuleBodyCreatorVisitor ruleBodyCreatorVisitor(symbolTable, projectId, ruleSymbolMap);
    parsingDomain->GlobalNamespace()->Accept(ruleBodyCreatorVisitor);
}

std::unique_ptr<gendoc::dom::Document> GenerateContentXml(dom::Document* inputXml, dom::Document* solutionXml, const std::string& contentXmlFilePath, bool verbose,
    gendoc::cppsym::SymbolTable& symbolTable, std::vector<std::unique_ptr<gendoc::cppast::Node>>& asts, std::vector<gendoc::cppast::SourceFileNode*>& sourceFiles,
    ParsingDomain* parsingDomain, std::vector<std::unique_ptr<ParserFileContent>>& parserFileContents, std::unordered_map<int, std::u32string>& parserProjectMap)
{
    std::unique_ptr<gendoc::dom::Document> contentXml;
    std::unique_ptr<xpath::XPathObject> projects = xpath::Evaluate(U"/solution/project", solutionXml);
    int sourceFileIndex = 0;
    int parserSourceFileIndex = 0;
    std::unordered_map<std::string, SourceFileNode*> allSourceFiles;
    std::unordered_map<Rule*, RuleSymbol*> ruleSymbolMap;
    if (projects->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* projectNodeSet = static_cast<xpath::XPathNodeSet*>(projects.get());
        int n = projectNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* projectNode = (*projectNodeSet)[i];
            int projectId = 10000 + i;
            if (projectNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* projectElement = static_cast<dom::Element*>(projectNode);
                std::u32string projectName = projectElement->GetAttribute(U"name");
                std::unique_ptr<xpath::XPathObject> component = xpath::Evaluate(U"/solution/components/component[@name=\"" + projectName + U"\"]", inputXml);
                if (component->Type() == xpath::XPathObjectType::nodeSet)
                {
                    xpath::XPathNodeSet* componentNodeSet = static_cast<xpath::XPathNodeSet*>(component.get());
                    if (componentNodeSet->Length() == 1)
                    {
                        if (verbose)
                        {
                            std::cout << "prosessing project '" << ToUtf8(projectName) << "'..." << std::endl;
                        }
                        std::unique_ptr<xpath::XPathObject> sources = xpath::Evaluate(U"sources/source", projectNode);
                        std::vector<std::pair<std::string, std::string>> sourceFilePaths;
                        if (sources->Type() == xpath::XPathObjectType::nodeSet)
                        {
                            xpath::XPathNodeSet* sourceNodeSet = static_cast<xpath::XPathNodeSet*>(sources.get());
                            int m = sourceNodeSet->Length();
                            for (int j = 0; j < m; ++j)
                            {
                                dom::Node* sourceNode = (*sourceNodeSet)[j];
                                if (sourceNode->GetNodeType() == dom::NodeType::elementNode)
                                {
                                    dom::Element* sourceElement = static_cast<dom::Element*>(sourceNode);
                                    std::string sourceFilePath = ToUtf8(sourceElement->GetAttribute(U"filePath"));
                                    std::string relativeSourceFilePath = ToUtf8(sourceElement->GetAttribute(U"relativeFilePath"));
                                    sourceFilePaths.push_back(std::make_pair(sourceFilePath, relativeSourceFilePath));
                                }
                            }
                        }
                        std::unique_ptr<gendoc::cppast::Node> ast = ParseSourceFiles(sourceFilePaths, sourceFiles, allSourceFiles, verbose, sourceFileIndex, projectName);
                        if (ast)
                        {
                            if (verbose)
                            {
                                std::cout << "creating symbols..." << std::endl;
                            }
                            SymbolCreator symbolCreator(symbolTable, projectName);
                            ast->Accept(symbolCreator);
                            if (verbose)
                            {
                                std::cout << "binding types..." << std::endl;
                            }
                            TypeBinder typeBinder(symbolTable);
                            ast->Accept(typeBinder);
                            if (verbose)
                            {
                                std::cout << "resolving override sets..." << std::endl;
                            }
                            ResolveOverrideSets(typeBinder.ClassesHavingVirtualFunctions());
                            if (verbose)
                            {
                                std::cout << "resolving symbols..." << std::endl;
                            }
                            std::vector<std::unique_ptr<BoundSourceFile>> sourceFiles = typeBinder.SourceFiles();
                            std::unordered_map<SourceFileNode*, BoundSourceFile*> sourceFileMap = typeBinder.SourceFileMap();
                            StatementBinder statementBinder(symbolTable, sourceFileMap);
                            ast->Accept(statementBinder);
                            asts.push_back(std::move(ast));
                        }
                        std::unique_ptr<xpath::XPathObject> parserSources = xpath::Evaluate(U"parserSources/parserSource", projectNode);
                        std::vector<std::pair<std::string, std::string>> parserSourceFilePaths;
                        if (parserSources->Type() == xpath::XPathObjectType::nodeSet)
                        {
                            xpath::XPathNodeSet* parserSourceNodeSet = static_cast<xpath::XPathNodeSet*>(parserSources.get());
                            int m = parserSourceNodeSet->Length();
                            for (int j = 0; j < m; ++j)
                            {
                                dom::Node* parserSourceNode = (*parserSourceNodeSet)[j];
                                if (parserSourceNode->GetNodeType() == dom::NodeType::elementNode)
                                {
                                    dom::Element* parserSourceElement = static_cast<dom::Element*>(parserSourceNode);
                                    std::string sourceFilePath = ToUtf8(parserSourceElement->GetAttribute(U"filePath"));
                                    std::string relativeSourceFilePath = ToUtf8(parserSourceElement->GetAttribute(U"relativeFilePath"));
                                    parserSourceFilePaths.push_back(std::make_pair(sourceFilePath, relativeSourceFilePath));
                                }
                            }
                        }
                        ParseParserSourceFiles(parserSourceFilePaths, parsingDomain, parserFileContents, parserSourceFileIndex, projectId, projectName, parserProjectMap, verbose);
                        if (verbose)
                        {
                            std::cout << "processing grammars..." << std::endl;
                        }
                        ProcessGrammars(symbolTable, parsingDomain, projectId, projectName, parserProjectMap, ruleSymbolMap);
                    }
                }
            }
        }
    }
    contentXml.reset(new dom::Document());
    std::unique_ptr<dom::Element> solutionElement(new dom::Element(U"solution"));
    std::unique_ptr<dom::Element> symbolTableElement = symbolTable.ToDomElement();
    solutionElement->AppendChild(std::unique_ptr<dom::Node>(symbolTableElement.release()));
    std::ofstream contentXmlFile(contentXmlFilePath);
    CodeFormatter formatter(contentXmlFile);
    formatter.SetIndentSize(1);
    contentXml->AppendChild(std::unique_ptr<dom::Node>(solutionElement.release()));
    contentXml->Write(formatter);
    if (verbose)
    {
        std::cout << "==> " << contentXmlFilePath << std::endl;
    }
    return contentXml;
}

std::unique_ptr<gendoc::dom::Document> ReadContentXml(const std::string& contentXmlFilePath)
{
    return dom::ReadDocument(contentXmlFilePath);
}

} } // namespace gendoc::content
