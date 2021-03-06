// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_PARSING_PARSING_DOMAIN_INCLUDED
#define GENDOC_PARSING_PARSING_DOMAIN_INCLUDED
#include <gendoc/parsing/ParsingObject.hpp>
#include <string>
#include <stack>
#include <unordered_map>

namespace gendoc { namespace parsing {

class Scope;
class Grammar;
class Namespace;

typedef std::unordered_map<std::u32string, Grammar*> GrammarMap;
typedef GrammarMap::const_iterator GrammarMapIt;

class ParsingDomain: public ParsingObject
{
public:
    ParsingDomain();
    Scope* GetNamespaceScope(const std::u32string& fullNamespaceName);
    Grammar* GetGrammar(const std::u32string& grammarName);
    void AddGrammar(Grammar* grammar);
    void BeginNamespace(const std::u32string& ns);
    void EndNamespace();
    Namespace* GlobalNamespace() const { return globalNamespace; }
    Namespace* CurrentNamespace() const { return currentNamespace; }
    Scope* CurrentScope() const;
    void Accept(Visitor& visitor) override;
    int GetNextRuleId() { return nextRuleId++; }
    int GetNumRules() const { return nextRuleId; }
    GrammarMap& GetGrammarMap() { return grammarMap; }
    const std::u32string& ProjectPrefix() const { return projectPrefix; }
    void SetProjectPrefix(const std::u32string& projectPrefix_) { projectPrefix = projectPrefix_; }
private:
    GrammarMap grammarMap;
    Namespace* globalNamespace;
    Scope* globalScope;
    typedef std::unordered_map<std::u32string, Namespace*> NamespaceMap;
    typedef NamespaceMap::const_iterator NamespaceMapIt;
    NamespaceMap namespaceMap;
    std::stack<Namespace*> namespaceStack;
    Namespace* currentNamespace;
    int nextRuleId;
    std::u32string projectPrefix;
};

void RegisterParsingDomain(ParsingDomain* parsingDomain);
void ParsingDomainInit();
void ParsingDomainDone();

} } // namespace gendoc::parsing

#endif // GENDOC_PARSING_PARSING_DOMAIN_INCLUDED
