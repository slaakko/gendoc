// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/cppsym/FunctionSymbol.hpp>
#include <gendoc/cppsym/ParameterSymbol.hpp>
#include <gendoc/cppsym/TemplateSymbol.hpp>
#include <gendoc/util/Unicode.hpp>
#include <algorithm>

namespace gendoc { namespace cppsym {

using namespace gendoc::unicode;

FunctionDeclarationSymbol::FunctionDeclarationSymbol(const Span& span_, const std::u32string& groupName_, const std::u32string& name_, Specifier specifiers_) :
    ContainerSymbol(span_, name_), groupName(groupName_), specifiers(specifiers_), functionGroup(nullptr), returnType(nullptr), functionDefinition(nullptr)
{
}

std::unique_ptr<dom::Element> FunctionDeclarationSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> functionElement(new dom::Element(U"function_declaration"));
    return functionElement;
}

void FunctionDeclarationSymbol::AddMember(std::unique_ptr<Symbol>&& member)
{
    Symbol* symbol = member.get();
    if (symbol->IsParameterSymbol())
    {
        parameters.push_back(static_cast<ParameterSymbol*>(symbol));
    }
    ContainerSymbol::AddMember(std::move(member));
}

void FunctionDeclarationSymbol::AddSpecifiers(Specifier specifiers_)
{
    specifiers = specifiers | specifiers_;
}

ConstructorDeclarationSymbol::ConstructorDeclarationSymbol(const Span& span_, const std::u32string& name_, Specifier specifiers_) :
    FunctionDeclarationSymbol(span_, U"constructor", name_, specifiers_)
{
}

std::unique_ptr<dom::Element> ConstructorDeclarationSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> constructorDeclarationElement(new dom::Element(U"constructor_declaration"));
    return constructorDeclarationElement;
}

FunctionSymbol::FunctionSymbol(const Span& span_, const std::u32string& groupName_, const std::u32string& name_, Specifier specifiers_) :
    ContainerSymbol(span_, name_), index(0), groupName(groupName_), returnType(nullptr), specifiers(specifiers_), functionGroup(nullptr),
    declarationSpan()
{
}

void FunctionSymbol::AddMember(std::unique_ptr<Symbol>&& member)
{
    Symbol* symbol = member.get();
    if (symbol->IsParameterSymbol())
    {
        parameters.push_back(static_cast<ParameterSymbol*>(symbol));
    }
    ContainerSymbol::AddMember(std::move(member));
}

void FunctionSymbol::AddTemplateParameter(std::unique_ptr<TypeSymbol>&& templateParameter)
{
    templateParameters.push_back(std::move(templateParameter));
}

std::u32string FunctionSymbol::IdStr()
{
    std::u32string idStr = FullName();
    for (const std::unique_ptr<TypeSymbol>& templateParameter : templateParameters)
    {
        idStr.append(1, '_').append(templateParameter->Name());
    }
    for (ParameterSymbol* parameter : parameters)
    {
        idStr.append(1, '.').append(parameter->GetType()->Id());
    }
    if ((specifiers & Specifier::const_) != Specifier::none)
    {
        idStr.append(U".const");
    }
    if ((specifiers & Specifier::constExpr) != Specifier::none)
    {
        idStr.append(U".constExpr");
    }
    return idStr;
}

std::unique_ptr<dom::Element> FunctionSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> functionElement(new dom::Element(U"function"));
    if (!templateParameters.empty())
    {
        std::unique_ptr<dom::Element> templateParametersElement(new dom::Element(U"templateParameters"));
        for (const std::unique_ptr<TypeSymbol>& templateParameter : templateParameters)
        {
            std::unique_ptr<dom::Element> templateParameterElement = templateParameter->ToDomElement();
            templateParametersElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(templateParametersElement.release()));
    }
    if (!parameters.empty())
    {
        std::unique_ptr<dom::Element> parametersElement(new dom::Element(U"parameters"));
        for (ParameterSymbol* parameter : parameters)
        {
            std::unique_ptr<dom::Element> parameterElement = parameter->ToDomElement();
            parametersElement->AppendChild(std::unique_ptr<dom::Node>(parameterElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(parametersElement.release()));
    }
    if (returnType)
    {
        functionElement->SetAttribute(U"returnType", returnType->Id());
    }
    if (specifiers != Specifier::none)
    {
        functionElement->SetAttribute(U"specifiers", SpecifierStr(specifiers));
    }
    if (GetSpan().Valid())
    {
        functionElement->SetAttribute(U"definitionFile", ToUtf32(fileName));
        functionElement->SetAttribute(U"definitionLine", ToUtf32(std::to_string(GetSpan().LineNumber())));
        functionElement->SetAttribute(U"definitionFileId", fileId);
    }
    if (declarationSpan.Valid())
    {
        functionElement->SetAttribute(U"declarationFile", ToUtf32(declarationFileName));
        functionElement->SetAttribute(U"declarationLine", ToUtf32(std::to_string(declarationSpan.LineNumber())));
        functionElement->SetAttribute(U"declarationFileId", declarationFileId);
    }
    if (!overrides.empty())
    {
        std::unique_ptr<dom::Element> overridesElement(new dom::Element(U"overrides"));
        for (FunctionSymbol* override_ : overrides)
        {
            std::unique_ptr<dom::Element> overrideElement(new dom::Element(U"override"));
            overrideElement->SetAttribute(U"functionId", override_->Id());
            overridesElement->AppendChild(std::unique_ptr<dom::Node>(overrideElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(overridesElement.release()));
    }
    if (!overridden.empty())
    {
        std::unique_ptr<dom::Element> overriddenFunctionsElement(new dom::Element(U"overriddenFunctions"));
        for (FunctionSymbol* overridden_ : overridden)
        {
            std::unique_ptr<dom::Element> overriddenFunctionElement(new dom::Element(U"overriddenFunction"));
            overriddenFunctionElement->SetAttribute(U"functionId", overridden_->Id());
            overriddenFunctionsElement->AppendChild(std::unique_ptr<dom::Node>(overriddenFunctionElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(overriddenFunctionsElement.release()));
    }
    if (!calls.empty())
    {
        std::unique_ptr<dom::Element> callsElement(new dom::Element(U"calls"));
        for (FunctionSymbol* function : calls)
        {
            std::unique_ptr<dom::Element> callElement(new dom::Element(U"call"));
            callElement->SetAttribute(U"functionId", function->Id());
            callsElement->AppendChild(std::unique_ptr<dom::Node>(callElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(callsElement.release()));
    }
    if (!calledBy.empty())
    {
        std::unique_ptr<dom::Element> calledByFunctionsElement(new dom::Element(U"calledByFunctions"));
        for (FunctionSymbol* function : calledBy)
        {
            std::unique_ptr<dom::Element> calledByFunctionElement(new dom::Element(U"calledByFunction"));
            calledByFunctionElement->SetAttribute(U"functionId", function->Id());
            calledByFunctionsElement->AppendChild(std::unique_ptr<dom::Node>(calledByFunctionElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(calledByFunctionsElement.release()));
    }
    return functionElement;
}

bool FunctionSymbol::NameBefore(Symbol* that) 
{
    if (ContainerSymbol::NameBefore(that)) return true;
    if (that->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbolThat = static_cast<FunctionSymbol*>(that);
        if (functionSymbolThat->ContainerSymbol::NameBefore(this)) return false;
        int np = parameters.size();
        int nt = functionSymbolThat->parameters.size();
        int n = std::min(np, nt);
        for (int i = 0; i < n; ++i)
        {
            ParameterSymbol* p = parameters[i];
            ParameterSymbol* t = functionSymbolThat->parameters[i];
            TypeSymbol* pType = p->GetType();
            TypeSymbol* tType = t->GetType();
            if (pType->NameBefore(tType)) return true;
            if (tType->NameBefore(pType)) return false;
        }
        if (np < nt) return true;
        if (nt < np) return false;
    }
    return false;
}

void FunctionSymbol::AddSpecifiers(Specifier specifiers_)
{
    specifiers = specifiers | specifiers_;
}

void FunctionSymbol::AddOverride(FunctionSymbol* override_)
{
    overrides.insert(override_);
}

void FunctionSymbol::AddOverridden(FunctionSymbol* overridden_)
{
    overridden.insert(overridden_);
}

void FunctionSymbol::AddToCalls(FunctionSymbol* function)
{
    calls.insert(function);
}

void FunctionSymbol::AddToCalledBy(FunctionSymbol* function)
{
    calledBy.insert(function);
}

ConstructorSymbol::ConstructorSymbol(const Span& span_, const std::u32string& name_, Specifier specifiers_) : FunctionSymbol(span_, U"constructor", name_, specifiers_)
{
}

std::unique_ptr<dom::Element> ConstructorSymbol::CreateElement()
{
    std::unique_ptr<dom::Element> constructorElement(new dom::Element(U"constructor"));
    if (!Parameters().empty())
    {
        std::unique_ptr<dom::Element> parametersElement(new dom::Element(U"parameters"));
        for (ParameterSymbol* parameter : Parameters())
        {
            std::unique_ptr<dom::Element> parameterElement = parameter->ToDomElement();
            parametersElement->AppendChild(std::unique_ptr<dom::Node>(parameterElement.release()));
        }
        constructorElement->AppendChild(std::unique_ptr<dom::Node>(parametersElement.release()));
    }
    if (Specifiers() != Specifier::none)
    {
        constructorElement->SetAttribute(U"specifiers", SpecifierStr(Specifiers()));
    }
    if (GetSpan().Valid())
    {
        constructorElement->SetAttribute(U"definitionFile", ToUtf32(FileName()));
        constructorElement->SetAttribute(U"definitionLine", ToUtf32(std::to_string(GetSpan().LineNumber())));
        constructorElement->SetAttribute(U"definitionFileId", FileId());
    }
    if (DeclarationSpan().Valid())
    {
        constructorElement->SetAttribute(U"declarationFile", ToUtf32(DeclarationFileName()));
        constructorElement->SetAttribute(U"declarationLine", ToUtf32(std::to_string(DeclarationSpan().LineNumber())));
        constructorElement->SetAttribute(U"declarationFileId", DeclarationFileId());
    }
    return constructorElement;
}

FunctionGroupSymbol::FunctionGroupSymbol(const Span& span_, const std::u32string& name_) : Symbol(span_, name_)
{
}

void FunctionGroupSymbol::AddFunction(std::unique_ptr<FunctionSymbol>&& function)
{
    function->SetIndex(functions.size());
    function->SetParent(Parent());
    function->SetFunctionGroup(this);
    functions.push_back(std::move(function));
}

void FunctionGroupSymbol::AddFunctionDeclaration(std::unique_ptr<FunctionDeclarationSymbol>&& functionDeclaration)
{
    functionDeclaration->SetParent(Parent());
    functionDeclaration->SetFunctionGroup(this);
    functionDeclarations.push_back(std::move(functionDeclaration));
}

FunctionDeclarationSymbol* FunctionGroupSymbol::GetFunctionDeclaration(const std::vector<ParameterSymbol*>& parameters, Specifier specifiers)
{
    FunctionDeclarationSymbol* functionDeclarationSymbol = nullptr;
    for (const std::unique_ptr<FunctionDeclarationSymbol>& functionDeclaration : functionDeclarations)
    {
        int n = functionDeclaration->Arity();
        if (n == parameters.size())
        {
            bool match = true;
            bool declarationConst = (functionDeclaration->Specifiers() & Specifier::const_) != Specifier::none;
            bool const_ = (specifiers & Specifier::const_) != Specifier::none;
            if (declarationConst != const_)
            {
                match = false;
            }
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    ParameterSymbol* declarationParameter = functionDeclaration->Parameters()[i];
                    ParameterSymbol* parameter = parameters[i];
                    if (!declarationParameter->GetType() || declarationParameter->GetType()->Id() != parameter->GetType()->Id())
                    {
                        match = false;
                        break;
                    }
                }
            }
            if (match)
            {
                if (functionDeclarationSymbol)
                {
                    throw std::runtime_error("already has match");
                }
                functionDeclarationSymbol = functionDeclaration.get();
            }
        }
    }
    return functionDeclarationSymbol;
}

std::unique_ptr<dom::Element> FunctionGroupSymbol::CreateElement()
{
    return std::unique_ptr<dom::Element>(new dom::Element(U"function_group"));
}

struct FunctionMatch
{
    FunctionMatch() : matchValues() {}
    std::vector<int> matchValues;
};

struct BetterFunctionMatch
{
    bool operator()(const std::pair<FunctionSymbol*, FunctionMatch>& left, const std::pair<FunctionSymbol*, FunctionMatch>& right) const
    {
        int arity = left.second.matchValues.size();
        if (arity != right.second.matchValues.size())
        {
            throw std::runtime_error("wrong arity");
        }
        int betterLeftMatchValues = 0;
        int betterRightMatchValues = 0;
        for (int i = 0; i < arity; ++i)
        {
            int leftMatchValue = left.second.matchValues[i];
            int rightMatchValue = right.second.matchValues[i];
            if (leftMatchValue > rightMatchValue)
            {
                ++betterLeftMatchValues;
            }
            else if (rightMatchValue > leftMatchValue)
            {
                ++betterRightMatchValues;
            }
        }
        if (betterLeftMatchValues > betterRightMatchValues)
        {
            return true;
        }
        else if (betterRightMatchValues > betterLeftMatchValues)
        {
            return false;
        }
        else
        {
            for (int i = 0; i < arity; ++i)
            {
                int leftMatchValue = left.second.matchValues[i];
                int rightMatchValue = right.second.matchValues[i];
                if (leftMatchValue > 1 || rightMatchValue > 1)
                {
                    int x = 0;
                }
            }
        }
        return false;
    }
};

FunctionSymbol* FunctionGroupSymbol::ResolveOverload(const std::vector<Symbol*>& argumentSymbols)
{
    int arity = argumentSymbols.size();
    std::vector<FunctionSymbol*> viableFunctions;
    for (const std::unique_ptr<FunctionSymbol>& function : functions)
    {
        if (function->Arity() == arity)
        {
            viableFunctions.push_back(function.get());
        }
    }
    if (viableFunctions.empty())
    {
        return nullptr;
    }
    else if (viableFunctions.size() == 1)
    {
        return viableFunctions.front();
    }
    else
    {
        int x = 0;
        std::vector<std::pair<FunctionSymbol*, FunctionMatch>> functionMatches;
        int n = viableFunctions.size();
        for (int i = 0; i < n; ++i)
        {
            FunctionSymbol* viableFunction = viableFunctions[i];
            FunctionMatch functionMatch;
            for (int i = 0; i < arity; ++i)
            {
                ParameterSymbol* parameter = viableFunction->Parameters()[i];
                TypeSymbol* parameterType = parameter->GetType();
                Symbol* argumentSymbol = argumentSymbols[i];
                TypeSymbol* argumentType = nullptr;
                if (argumentSymbol)
                {
                    argumentType = argumentSymbol->GetType();
                }
                int matchValue = parameterType->MatchValue(argumentType);
                functionMatch.matchValues.push_back(matchValue);
            }
            functionMatches.push_back(std::make_pair(viableFunction, functionMatch));
        }
        std::sort(functionMatches.begin(), functionMatches.end(), BetterFunctionMatch());
        if (BetterFunctionMatch()(functionMatches[0], functionMatches[1]))
        {
            FunctionSymbol* bestMatch = functionMatches[0].first;
            return bestMatch;
        }
        return nullptr;
    }
}

} } // namespace gendoc::cppsym
