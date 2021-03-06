// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/syntax/CodeGeneratorVisitor.hpp>
#include <gendoc/util/TextUtils.hpp>
#include <gendoc/util/Unicode.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Rule.hpp>
#include <gendoc/parsing/Action.hpp>
#include <gendoc/parsing/Nonterminal.hpp>
#include <gendoc/parsing/Namespace.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using gendoc::unicode::ToUtf8;
using gendoc::unicode::ToUtf32;
using gendoc::util::StringStr;
using gendoc::util::CharStr;
using gendoc::util::Replace;

class KeywordListCreator : public gendoc::parsing::Visitor
{
public:
    KeywordListCreator (CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
        cppFormatter(cppFormatter_), hppFormatter(hppFormatter_), keywordListNumber(0), keywordRuleListNumber(0)
    {
    }
    void Visit(KeywordListParser& parser) override
    {
        std::string keywordVecName = "keywords" + std::to_string(keywordListNumber);
        ++keywordListNumber;
        hppFormatter.WriteLine("std::vector<std::u32string> " + keywordVecName + ";");
        KeywordListParser::KeywordSetIt e = parser.Keywords().end();
        for (KeywordListParser::KeywordSetIt i = parser.Keywords().begin(); i != e; ++i)
        {
            std::string keywordStr = "ToUtf32(\"" + StringStr(ToUtf8(*i)) + "\")";
            cppFormatter.WriteLine(keywordVecName + ".push_back(" + keywordStr + ");");
        }
        parser.KeywordVecName() = ToUtf32(keywordVecName);
    }
private:
    CodeFormatter& cppFormatter;
    CodeFormatter& hppFormatter;
    int keywordListNumber;
    int keywordRuleListNumber;
};

CodeGeneratorVisitor::CodeGeneratorVisitor(CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
    cppFormatter(cppFormatter_), hppFormatter(hppFormatter_)
{
}

void CodeGeneratorVisitor::BeginVisit(Grammar& grammar)
{
    std::string grammarPtrType = ToUtf8(grammar.Name()) + "*";
    hppFormatter.WriteLine("class " + ToUtf8(grammar.Name()) + " : public gendoc::parsing::Grammar");
    hppFormatter.WriteLine("{");
    hppFormatter.WriteLine("public:");
    hppFormatter.IncIndent();
    hppFormatter.WriteLine("static " + grammarPtrType + " Create();");
    hppFormatter.WriteLine("static " + grammarPtrType + " Create(gendoc::parsing::ParsingDomain* parsingDomain);");

    std::string valueTypeName;
    std::string parameters;
    if (grammar.StartRule())
    {
        valueTypeName = !grammar.StartRule()->ValueTypeName().empty() ? ToUtf8(grammar.StartRule()->ValueTypeName()) : "void";
        parameters = "";
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                parameters.append(", " + ToUtf8(attr.TypeName()) + " " + ToUtf8(attr.Name()));
            }
            hppFormatter.WriteLine(valueTypeName + " Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName" + parameters + ");");
        }
    }
    hppFormatter.DecIndent();
    hppFormatter.WriteLine("private:");
    hppFormatter.IncIndent();

    cppFormatter.WriteLine(grammarPtrType + " " + ToUtf8(grammar.Name()) + "::Create()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("return Create(new gendoc::parsing::ParsingDomain());");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    cppFormatter.WriteLine(grammarPtrType + " " + ToUtf8(grammar.Name()) + "::Create(gendoc::parsing::ParsingDomain* parsingDomain)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("RegisterParsingDomain(parsingDomain);");
    cppFormatter.WriteLine(grammarPtrType + " grammar(new " + ToUtf8(grammar.Name()) + "(parsingDomain));");
    cppFormatter.WriteLine("parsingDomain->AddGrammar(grammar);");
    cppFormatter.WriteLine("grammar->CreateRules();");
    cppFormatter.WriteLine("grammar->Link();");
    cppFormatter.WriteLine("return grammar;");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    std::string grammarNamespaceName = ToUtf8(grammar.Ns()->FullName());
    cppFormatter.WriteLine(ToUtf8(grammar.Name()) + "::" + ToUtf8(grammar.Name()) + "(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32(\"" + ToUtf8(grammar.Name()) + 
        "\"), parsingDomain_->GetNamespaceScope(ToUtf32(\"" + grammarNamespaceName + "\")), parsingDomain_)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("SetOwner(0);");
    KeywordListCreator creator(cppFormatter, hppFormatter);
    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        rule->Accept(creator);
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    if (grammar.StartRule())
    {
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            cppFormatter.WriteLine(valueTypeName + " " + ToUtf8(grammar.Name()) + 
                "::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName" + parameters + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("gendoc::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());");
            cppFormatter.WriteLine("std::unique_ptr<gendoc::parsing::XmlLog> xmlLog;");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog.reset(new gendoc::parsing::XmlLog(*Log(), MaxLogLineLength()));");
            cppFormatter.WriteLine("scanner.SetLog(xmlLog.get());");
            cppFormatter.WriteLine("xmlLog->WriteBeginRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("gendoc::parsing::ObjectStack stack;");
            cppFormatter.WriteLine("std::unique_ptr<gendoc::parsing::ParsingData> parsingData(new gendoc::parsing::ParsingData(GetParsingDomain()->GetNumRules()));");
            cppFormatter.WriteLine("scanner.SetParsingData(parsingData.get());");

            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                cppFormatter.WriteLine("stack.push(std::unique_ptr<gendoc::parsing::Object>(new ValueObject<" + ToUtf8(attr.TypeName()) + ">(" + ToUtf8(attr.Name()) + ")));");
            }

            cppFormatter.WriteLine("gendoc::parsing::Match match = gendoc::parsing::Grammar::Parse(scanner, stack, parsingData.get());");
            cppFormatter.WriteLine("gendoc::parsing::Span stop = scanner.GetSpan();");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog->WriteEndRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("if (!match.Hit() || stop.Start() != int(end - start))");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("if (StartRule())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw gendoc::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("else");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw gendoc::parsing::ParsingException(\"grammar '\" + ToUtf8(Name()) + \"' has no start rule\", fileName, scanner.GetSpan(), start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            if (valueTypeName != "void")
            {
                cppFormatter.WriteLine("std::unique_ptr<gendoc::parsing::Object> value = std::move(stack.top());");
                cppFormatter.WriteLine(valueTypeName + " result = *static_cast<gendoc::parsing::ValueObject<" + valueTypeName + ">*>(value.get());");
                cppFormatter.WriteLine("stack.pop();");
                cppFormatter.WriteLine("return result;");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.NewLine();
        }
    }
    n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            cppFormatter.WriteLine("class " + ToUtf8(grammar.Name()) + "::" + ToUtf8(rule->SpecializedTypeName()) + " : public gendoc::parsing::Rule");
            cppFormatter.WriteLine("{");
            cppFormatter.WriteLine("public:");
            cppFormatter.IncIndent();
            // constructor:
            cppFormatter.WriteLine(ToUtf8(rule->SpecializedTypeName()) + "(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            int m = int(rule->InheritedAttributes().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("AddInheritedAttribute(AttrOrVariable(ToUtf32(\"" + ToUtf8(attr.TypeName()) + "\"), ToUtf32(\"" + ToUtf8(attr.Name()) + "\")));");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("SetValueTypeName(ToUtf32(\"" + ToUtf8(rule->ValueTypeName()) + "\"));");
            }
            m = int(rule->LocalVariables().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& var = rule->LocalVariables()[i];
                cppFormatter.WriteLine("AddLocalVariable(AttrOrVariable(ToUtf32(\"" + ToUtf8(var.TypeName()) + "\"), ToUtf32(\"" + ToUtf8(var.Name()) + "\")));");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // enter:
            cppFormatter.WriteLine("void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("parsingData->PushContext(Id(), new Context());");
            cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
            m = int(rule->InheritedAttributes().size());
            for (int i = m - 1; i >= 0; --i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("std::unique_ptr<gendoc::parsing::Object> " + ToUtf8(attr.Name()) + "_value = std::move(stack.top());");
                cppFormatter.WriteLine("context->" + ToUtf8(attr.Name()) + " = *static_cast<gendoc::parsing::ValueObject<" + ToUtf8(attr.TypeName()) + ">*>(" + ToUtf8(attr.Name()) + "_value.get());");
                cppFormatter.WriteLine("stack.pop();");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            // leave:
            cppFormatter.WriteLine("void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                cppFormatter.WriteLine("if (matched)");
                cppFormatter.WriteLine("{");
                cppFormatter.IncIndent();
                cppFormatter.WriteLine("stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<" + ToUtf8(rule->ValueTypeName()) + ">(context->value)));");
                cppFormatter.DecIndent();
                cppFormatter.WriteLine("}");
            }
            cppFormatter.WriteLine("parsingData->PopContext(Id());");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // link:
            cppFormatter.WriteLine("void Link() override");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("gendoc::parsing::ActionParser* " + ToUtf8(action->VariableName()) + " = GetAction(ToUtf32(\"" + ToUtf8(action->Name()) + "\"));");
                cppFormatter.WriteLine(ToUtf8(action->VariableName()) + "->SetAction(new gendoc::parsing::MemberParsingAction<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                    ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(action->MethodName()) + "));");
                if (action->FailCode())
                {
                    cppFormatter.WriteLine(ToUtf8(action->VariableName()) + "->SetFailureAction(new gendoc::parsing::MemberFailureAction<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                        ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(action->MethodName()) + "Fail));");
                }
            }
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    cppFormatter.WriteLine("gendoc::parsing::NonterminalParser* " + ToUtf8(nonterminal->VariableName()) + " = GetNonterminal(ToUtf32(\"" + ToUtf8(nonterminal->Name()) + "\"));");
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine(ToUtf8(nonterminal->VariableName()) + "->SetPreCall(new gendoc::parsing::MemberPreCall<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                            ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(nonterminal->PreCallMethodName()) + "));");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine(ToUtf8(nonterminal->VariableName()) + "->SetPostCall(new gendoc::parsing::MemberPostCall<" + ToUtf8(rule->SpecializedTypeName()) + ">(this, &" + 
                            ToUtf8(rule->SpecializedTypeName()) + "::" + ToUtf8(nonterminal->PostCallMethodName()) + "));");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // action methods:
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("void " + ToUtf8(action->MethodName()) +
                    "(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)");
                cppFormatter.SetStart();
                cppFormatter.SetStartText("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                action->SuccessCode()->Print(cppFormatter);
                if (action->FailCode())
                {
                    cppFormatter.WriteLine("void " + ToUtf8(action->MethodName()) + "Fail(ParsingData* parsingData)");
                    cppFormatter.SetStart();
                    cppFormatter.SetStartText("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                    action->FailCode()->Print(cppFormatter);
                }
            }

            // pre-post calls:
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine("void " + ToUtf8(nonterminal->PreCallMethodName()) + "(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                        int p = int(nonterminal->Arguments().size());
                        for (int j = 0; j < p; ++j)
                        {
                            gendoc::codedom::CppObject* argument = nonterminal->Arguments()[j];
                            std::string argumentTypeName = ToUtf8(nonterminal->GetRule()->InheritedAttributes()[j].TypeName());
                            cppFormatter.Write("stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<" + argumentTypeName + ">(");
                            argument->Print(cppFormatter);
                            cppFormatter.WriteLine(")));");
                        }
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine("void " + ToUtf8(nonterminal->PostCallMethodName()) + "(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("Context* context = static_cast<Context*>(parsingData->GetContext(Id()));");
                        cppFormatter.WriteLine("if (matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("std::unique_ptr<gendoc::parsing::Object> " + ToUtf8(nonterminal->ValueFieldName()) + "_value = std::move(stack.top());");
                        cppFormatter.WriteLine("context->" + ToUtf8(nonterminal->ValueFieldName()) + " = *static_cast<gendoc::parsing::ValueObject<" + 
                            ToUtf8(nonterminal->GetRule()->ValueTypeName()) + ">*>(" + ToUtf8(nonterminal->ValueFieldName()) + "_value.get());");
                        cppFormatter.WriteLine("stack.pop();");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("private:");
            cppFormatter.IncIndent();

            // Context:
            cppFormatter.WriteLine("struct Context : gendoc::parsing::Context");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.Write("Context(): ");
            bool first = true;
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.Write(ToUtf8(attr.Name()) + "()");
            }
            if (!rule->ValueTypeName().empty())
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                cppFormatter.Write("value()");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.Write(ToUtf8(var.Name()) + "()");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    if (first) { first = false; } else { cppFormatter.Write(", "); }
                    cppFormatter.Write(ToUtf8(nonterminal->ValueFieldName()) + "()");
                }
            }
            cppFormatter.WriteLine(" {}");
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.WriteLine(ToUtf8(attr.TypeName()) + " " + ToUtf8(attr.Name()) + ";");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine(ToUtf8(rule->ValueTypeName()) + " value;");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.WriteLine(ToUtf8(var.TypeName()) + " " + ToUtf8(var.Name()) + ";");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    cppFormatter.WriteLine(ToUtf8(nonterminal->GetRule()->ValueTypeName()) + " " + ToUtf8(nonterminal->ValueFieldName()) + ";");
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.NewLine();
        }
    }

    cppFormatter.WriteLine("void " + ToUtf8(grammar.Name()) + "::GetReferencedGrammars()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    int grammarIndex = 0;
    if (!grammar.GrammarReferences().empty())
    {
        cppFormatter.WriteLine("gendoc::parsing::ParsingDomain* pd = GetParsingDomain();");
        GrammarSetIt e = grammar.GrammarReferences().cend();
        for (GrammarSetIt i = grammar.GrammarReferences().cbegin(); i != e; ++i)
        {
            Grammar* grammarReference = *i;
            std::string grammarReferenceName = ToUtf8(grammarReference->FullName());
            std::string grammarVar = "grammar" + std::to_string(grammarIndex);
            ++grammarIndex;
            cppFormatter.WriteLine("gendoc::parsing::Grammar* " + grammarVar + " = pd->GetGrammar(ToUtf32(\"" + grammarReferenceName + "\"));");
            cppFormatter.WriteLine("if (!" + grammarVar + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine(grammarVar + " = " + Replace(grammarReferenceName, ".", "::") + "::Create(pd);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("AddGrammarReference(" + grammarVar + ");");
        }
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();
    cppFormatter.WriteLine("void " + ToUtf8(grammar.Name()) + "::CreateRules()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
}

void CodeGeneratorVisitor::EndVisit(Grammar& grammar)
{
    if (!grammar.StartRuleName().empty())
    {
        cppFormatter.WriteLine("SetStartRuleName(ToUtf32(\"" + ToUtf8(grammar.StartRuleName()) + "\"));");
    }
    if (!grammar.SkipRuleName().empty())
    {
        cppFormatter.WriteLine("SetSkipRuleName(ToUtf32(\"" + ToUtf8(grammar.SkipRuleName()) + "\"));");
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.WriteLine();

    hppFormatter.WriteLine(ToUtf8(grammar.Name()) + "(gendoc::parsing::ParsingDomain* parsingDomain_);");
    hppFormatter.WriteLine("void CreateRules() override;");
    hppFormatter.WriteLine("void GetReferencedGrammars() override;");

    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            hppFormatter.WriteLine("class " + ToUtf8(rule->SpecializedTypeName()) + ";");
        }
    }

    hppFormatter.DecIndent();
    hppFormatter.WriteLine("};");
    hppFormatter.WriteLine();
}

void CodeGeneratorVisitor::Visit(CharParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::CharParser('");
    cppFormatter.Write(ToUtf8(CharStr(parser.GetChar())));
    cppFormatter.Write("')");
}

void CodeGeneratorVisitor::Visit(StringParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::StringParser(ToUtf32(\"" + ToUtf8(StringStr(parser.GetString())) + "\"))");
}

void CodeGeneratorVisitor::Visit(CharSetParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::CharSetParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Set())) + "\")");
    if (parser.Inverse())
    {
        cppFormatter.Write(", true)");
    }
    else
    {
        cppFormatter.Write(")");
    }
}

void CodeGeneratorVisitor::Visit(KeywordParser& parser)
{
    if (parser.ContinuationRuleName().empty())
    {
        cppFormatter.Write("new gendoc::parsing::KeywordParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Keyword())) + "\"))");
    }
    else
    {
        cppFormatter.Write("new gendoc::parsing::KeywordParser(ToUtf32(\"" + ToUtf8(StringStr(parser.Keyword())) + "\"), ToUtf32(\"" + ToUtf8(parser.ContinuationRuleName()) + "\"))");
    }
}

void CodeGeneratorVisitor::Visit(KeywordListParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::KeywordListParser(ToUtf32(\"" + ToUtf8(StringStr(parser.SelectorRuleName())) + "\"), " + ToUtf8(parser.KeywordVecName()) + ")");
}

void CodeGeneratorVisitor::Visit(RangeParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::RangeParser(" + std::to_string(parser.Start()) + ", " + std::to_string(parser.End()) + ")");
}

void CodeGeneratorVisitor::Visit(EmptyParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::EmptyParser()");
}

void CodeGeneratorVisitor::Visit(SpaceParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::SpaceParser()");
}

void CodeGeneratorVisitor::Visit(LetterParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::LetterParser()");
}

void CodeGeneratorVisitor::Visit(UpperLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::UpperLetterParser()");
}

void CodeGeneratorVisitor::Visit(LowerLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::LowerLetterParser()");
}

void CodeGeneratorVisitor::Visit(TitleLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::TitleLetterParser()");
}

void CodeGeneratorVisitor::Visit(ModifierLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ModifierLetterParser()");
}

void CodeGeneratorVisitor::Visit(OtherLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::OtherLetterParser()");
}

void CodeGeneratorVisitor::Visit(CasedLetterParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::CasedLetterParser()");
}

void CodeGeneratorVisitor::Visit(DigitParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::DigitParser()");
}

void CodeGeneratorVisitor::Visit(HexDigitParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::HexDigitParser()");
}

void CodeGeneratorVisitor::Visit(MarkParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::MarkParser()");
}

void CodeGeneratorVisitor::Visit(NonspacingMarkParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::NonspacingMarkParser()");
}

void CodeGeneratorVisitor::Visit(SpacingMarkParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::SpacingMarkParser()");
}

void CodeGeneratorVisitor::Visit(EnclosingMarkParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::EnclosingMarkParser()");
}

void CodeGeneratorVisitor::Visit(NumberParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::NumberParser()");
}

void CodeGeneratorVisitor::Visit(DecimalNumberParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::DecimalNumberParser()");
}

void CodeGeneratorVisitor::Visit(LetterNumberParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::LetterNumberParser()");
}

void CodeGeneratorVisitor::Visit(OtherNumberParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::OtherNumberParser()");
}

void CodeGeneratorVisitor::Visit(PunctuationParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::PunctuationParser()");
}

void CodeGeneratorVisitor::Visit(ConnectorPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ConnectorPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(DashPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::DashPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(OpenPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::OpenPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(ClosePunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ClosePunctuationParser()");
}

void CodeGeneratorVisitor::Visit(InitialPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::InitialPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(FinalPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::FinalPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(OtherPunctuationParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::OtherPunctuationParser()");
}

void CodeGeneratorVisitor::Visit(SymbolParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::SymbolParser()");
}

void CodeGeneratorVisitor::Visit(MathSymbolParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::MathSymbolParser()");
}

void CodeGeneratorVisitor::Visit(CurrencySymbolParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::CurrencySymbolParser()");
}

void CodeGeneratorVisitor::Visit(ModifierSymbolParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ModifierSymbolParser()");
}

void CodeGeneratorVisitor::Visit(OtherSymbolParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::OtherSymbolParser()");
}

void CodeGeneratorVisitor::Visit(SeparatorParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::SeparatorParser()");
}

void CodeGeneratorVisitor::Visit(SpaceSeparatorParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::SpaceSeparatorParser()");
}

void CodeGeneratorVisitor::Visit(LineSeparatorParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::LineSeparatorParser()");
}

void CodeGeneratorVisitor::Visit(ParagraphSeparatorParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ParagraphSeparatorParser()");
}

void CodeGeneratorVisitor::Visit(OtherParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::OtherParser()");
}

void CodeGeneratorVisitor::Visit(ControlParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::ControlParser()");
}

void CodeGeneratorVisitor::Visit(FormatParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::FormatParser()");
}

void CodeGeneratorVisitor::Visit(SurrogateParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::SurrogateParser()");
}

void CodeGeneratorVisitor::Visit(PrivateUseParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::PrivateUseParser()");
}

void CodeGeneratorVisitor::Visit(UnassignedParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::UnassignedParser()");
}

void CodeGeneratorVisitor::Visit(GraphicParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::GraphicParser()");
}

void CodeGeneratorVisitor::Visit(BaseCharParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::BaseCharParser()");
}

void CodeGeneratorVisitor::Visit(AlphabeticParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::AlphabeticParser()");
}

void CodeGeneratorVisitor::Visit(IdStartParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::IdStartParser()");
}

void CodeGeneratorVisitor::Visit(IdContParser& parser) 
{
    cppFormatter.Write("new gendoc::parsing::IdContParser()");
}

void CodeGeneratorVisitor::Visit(AnyCharParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::AnyCharParser()");
}

void CodeGeneratorVisitor::Visit(NonterminalParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::NonterminalParser(ToUtf32(\"" + ToUtf8(parser.Name()) + "\"), ToUtf32(\"" + ToUtf8(parser.RuleName()) + "\"), " + 
        std::to_string(parser.NumberOfArguments()) + ")");
}

void CodeGeneratorVisitor::Visit(RuleLink& link)
{
    cppFormatter.WriteLine("AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32(\"" + ToUtf8(link.Name()) + "\"), this, ToUtf32(\"" + ToUtf8(link.LinkedRuleName()) + "\")));");
}

void CodeGeneratorVisitor::BeginVisit(Rule& rule)
{
    if (rule.Specialized())
    {
        cppFormatter.Write("AddRule(new " + ToUtf8(rule.SpecializedTypeName()) + "(ToUtf32(\"" + ToUtf8(rule.Name()) + "\"), GetScope(), GetParsingDomain()->GetNextRuleId(),");
    }
    else
    {
        cppFormatter.Write("AddRule(new gendoc::parsing::Rule(ToUtf32(\"" + ToUtf8(rule.Name()) + "\"), GetScope(), GetParsingDomain()->GetNextRuleId(),");
    }
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(Rule& rule)
{
    cppFormatter.Write("));");
    cppFormatter.DecIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::BeginVisit(OptionalParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::OptionalParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(OptionalParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(PositiveParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::PositiveParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(PositiveParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(KleeneStarParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::KleeneStarParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(KleeneStarParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ActionParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::ActionParser(ToUtf32(\"" + ToUtf8(parser.Name()) + "\"),");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ActionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExpectationParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::ExpectationParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ExpectationParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(TokenParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::TokenParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(TokenParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(GroupingParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::GroupingParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(GroupingParser& parser) 
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(SequenceParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::SequenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(SequenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(SequenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(AlternativeParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::AlternativeParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(AlternativeParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(AlternativeParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(DifferenceParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::DifferenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(DifferenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(DifferenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(IntersectionParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::IntersectionParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(IntersectionParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(IntersectionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::ExclusiveOrParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ExclusiveOrParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ListParser& parser)
{
    cppFormatter.Write("new gendoc::parsing::ListParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ListParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ListParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

} } // namespace gendoc::syntax
