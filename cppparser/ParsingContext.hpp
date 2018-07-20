// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_CPPPARSER_PARSING_CONTEXT_INCLUDED
#define GENDOC_CPPPARSER_PARSING_CONTEXT_INCLUDED
#include <gendoc/cppast/SourceFile.hpp>
#include <stack>

namespace gendoc { namespace cppparser {

using namespace gendoc::cppast;

class ParsingContext
{
public:
    ParsingContext();
    bool ParsingTemplateId() const { return parsingTemplateId; }
    void BeginParsingTemplateId();
    void EndParsingTemplateId();
    bool InFunctionBody() const { return inFunctionBody; }
    void BeginFunctionBody();
    void EndFunctionBody();
    bool ParsingLvalue() const { return parsingLvalue; }
    void BeginParsingLvalue();
    void EndParsingLvalue();
    bool ParsingArguments() const { return parsingArguments; }
    void BeginParsingArguments();
    void EndParsingArguments();
    bool ParsingTemplateIdOrLvalue() const { return parsingTemplateId || parsingLvalue; }
    bool ParsingTemplateIdOrLvalueAndNotArguments() const { return (parsingTemplateId || parsingLvalue) && !parsingArguments; }
    bool ParsingLvalueAndNotArguments() const { return parsingLvalue && !parsingArguments; }
    void SetSourceFileNode(SourceFileNode* sourceFileNode_) { sourceFileNode = sourceFileNode_; }
    SourceFileNode* GetSourceFileNode() { return sourceFileNode; }
private:
    bool parsingTemplateId;
    std::stack<bool> parsingTemplateIdStack;
    bool inFunctionBody;
    std::stack<bool> inFunctionBodyStack;
    bool parsingLvalue;
    std::stack<bool> parsingLvalueStack;
    bool parsingArguments;
    std::stack<bool> parsingArgumentsStack;
    SourceFileNode* sourceFileNode;
};

} } // namespace gendoc::cppparser

#endif // GENDOC_CPPPARSER_PARSING_CONTEXT_INCLUDED
