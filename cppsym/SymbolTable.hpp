// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPSYM_SYMBOL_TABLE_INCLUDED
#define GENDOC_CPPSYM_SYMBOL_TABLE_INCLUDED
#include <gendoc/cppsym/NamespaceSymbol.hpp>
#include <gendoc/cppsym/ClassTypeSymbol.hpp>
#include <gendoc/cppsym/EnumerationSymbol.hpp>
#include <gendoc/cppsym/FunctionSymbol.hpp>
#include <gendoc/cppsym/DeclarationBlockSymbol.hpp>
#include <gendoc/cppsym/SimpleTypeSymbol.hpp>
#include <gendoc/cppsym/DerivedTypeSymbol.hpp>
#include <gendoc/cppsym/RuleSymbol.hpp>
#include <gendoc/cppast/Class.hpp>
#include <gendoc/cppast/Enumeration.hpp>
#include <gendoc/cppast/Function.hpp>
#include <gendoc/cppast/Declaration.hpp>
#include <gendoc/cppast/Declarator.hpp>
#include <gendoc/cppast/SimpleType.hpp>
#include <gendoc/cppast/Expression.hpp>
#include <gendoc/cppast/Template.hpp>
#include <gendoc/dom/Document.hpp>
#include <gendoc/dom/Element.hpp>

namespace gendoc { namespace cppsym {

using namespace gendoc::cppast;

class SymbolTable
{
public:
    SymbolTable();
    std::unique_ptr<dom::Document> ToDomDocument();
    std::unique_ptr<dom::Element> ToDomElement();
    std::unique_ptr<dom::Element> CreateTypesElement();
    void BeginContainer(ContainerSymbol* container_);
    void EndContainer();
    int BeginNameSequence(const std::vector<std::pair<std::u32string, int>>& nameSequence);
    void EndNameSequence(int n);
    void BeginNamespace(const std::u32string& namespaceName, const std::u32string& projectName);
    void EndNamespace();
    void BeginClass(ClassNode* classNode, const std::u32string& className, std::vector<std::unique_ptr<TypeSymbol>>& templateParameters);
    void EndClass(const std::u32string& projectName);
    void BeginGrammar(const Span& span, const std::u32string& grammarName);
    void EndGrammar(const std::u32string& projectName);
    RuleSymbol* AddRule(const Span& span, const std::u32string& ruleName);
    void BeginEnumType(EnumTypeNode* enumTypeNode, const std::u32string& enumTypeName);
    void EndEnumType(const std::u32string& projectName);
    void AddEnumerator(EnumeratorNode* enumeratorNode, const std::u32string& enumeratorName, const std::u32string& enumeratorValue);
    void BeginFunction(FunctionDeclaratorNode* functionDeclaratorNode, const std::u32string& groupName, const std::u32string& functionName, 
        std::vector<std::unique_ptr<TypeSymbol>>& templateParameters, Specifier specifiers);
    void EndFunction(const std::u32string& projectName);
    void BeginFunctionDeclaration(FunctionDeclaratorNode* functionDeclaratorNode, const std::u32string& groupName, const std::u32string& functionName, Specifier specifiers);
    void EndFunctionDeclaration();
    void AddParameter(ParameterNode* parameterNode, const std::u32string& parameterName);
    void BeginDeclarationBlock(Node* blockNode);
    void EndDeclarationBlock();
    void AddTypedef(TypedefNode* node, const std::u32string& typedefName, const std::u32string& projectName);
    void AddVariable(Node* node, const std::u32string& variableName, const std::u32string& projectName);
    void MapNode(Node* node, Symbol* symbol);
    void MapIdNodeSequence(Node* node, const std::vector<IdentifierNode*>& idNodeSequence);
    Symbol* GetSymbolNothrow(Node* node);
    Symbol* GetSymbol(Node* node);
    const std::vector<IdentifierNode*>& GetIdNodeSequence(Node* node);
    NamespaceSymbol* GlobalNs() { return &globalNs; }
    ContainerScope* GetContainerScope();
    TypeSymbol* MakeSimpleTypeSymbol(SimpleTypeNode& simpleTypeNode);
    TypeSymbol* MakeElaborateClassTypeSymbol(ClassKey classKey, TypeSymbol* classType);
    TypeSymbol* MakeDerivedTypeSymbol(std::vector<Derivation>& derivations, TypeSymbol* baseType);
    TypeSymbol* MakeExternalTypeSymbol(const Span& span, const std::u32string& name, ClassKey classKey);
    TypeSymbol* MakeClassGroupTypeSymbol(ClassGroupSymbol* classGroup);
    TypeSymbol* MakeClassTemplateSpecializationSymbol(const Span& span, TypeSymbol* primaryClassTemplate, TemplateIdNode* templateIdNode, 
        const std::vector<TypeSymbol*>& templateArguments, const std::vector<Node*>& templateArgumentNodes);
    TypeSymbol* MakeIntegerLiteralTypeSymbol(const Span& span, const std::u32string& valueName);
private:
    NamespaceSymbol globalNs;
    ContainerSymbol* container;
    std::stack<ContainerSymbol*> containerStack;
    std::unordered_map<Node*, Symbol*> nodeSymbolMap;
    std::unordered_map<Node*, std::vector<IdentifierNode*>> idNodeSequenceMap;
    int blockNumber;
    std::unordered_map<std::u32string, TypeSymbol*> idTypeMap;
    std::vector<std::unique_ptr<TypeSymbol>> types;
};

} } // namespace gendoc::cppsym

#endif // GENDOC_CPPSYM_SYMBOL_TABLE_INCLUDED
