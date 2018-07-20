#include "SolutionGrammar.hpp"
#include <gendoc/parsing/Action.hpp>
#include <gendoc/parsing/Rule.hpp>
#include <gendoc/parsing/ParsingDomain.hpp>
#include <gendoc/parsing/Primitive.hpp>
#include <gendoc/parsing/Composite.hpp>
#include <gendoc/parsing/Nonterminal.hpp>
#include <gendoc/parsing/Exception.hpp>
#include <gendoc/parsing/StdLib.hpp>
#include <gendoc/parsing/XmlLog.hpp>
#include <gendoc/util/Unicode.hpp>

namespace gendoc { namespace cppparser {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Solution* Solution::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Solution* Solution::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Solution* grammar(new Solution(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Solution::Solution(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Solution"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.cppparser")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::cppast::Solution* Solution::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
{
    gendoc::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<gendoc::parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new gendoc::parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    gendoc::parsing::ObjectStack stack;
    std::unique_ptr<gendoc::parsing::ParsingData> parsingData(new gendoc::parsing::ParsingData(GetParsingDomain()->GetNumRules()));
    scanner.SetParsingData(parsingData.get());
    gendoc::parsing::Match match = gendoc::parsing::Grammar::Parse(scanner, stack, parsingData.get());
    gendoc::parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw gendoc::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw gendoc::parsing::ParsingException("grammar '" + ToUtf8(Name()) + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<gendoc::parsing::Object> value = std::move(stack.top());
    gendoc::cppast::Solution* result = *static_cast<gendoc::parsing::ValueObject<gendoc::cppast::Solution*>*>(value.get());
    stack.pop();
    return result;
}

class Solution::SolutionRule : public gendoc::parsing::Rule
{
public:
    SolutionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::cppast::Solution*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::cppast::Solution*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SolutionRule>(this, &SolutionRule::A0Action));
        gendoc::parsing::NonterminalParser* solutionHeaderNonterminalParser = GetNonterminal(ToUtf32("SolutionHeader"));
        solutionHeaderNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SolutionRule>(this, &SolutionRule::PostSolutionHeader));
        gendoc::parsing::NonterminalParser* projectsNonterminalParser = GetNonterminal(ToUtf32("Projects"));
        projectsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SolutionRule>(this, &SolutionRule::PreProjects));
        gendoc::parsing::NonterminalParser* globalNonterminalParser = GetNonterminal(ToUtf32("Global"));
        globalNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<SolutionRule>(this, &SolutionRule::PreGlobal));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSolutionHeader;
    }
    void PostSolutionHeader(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSolutionHeader_value = std::move(stack.top());
            context->fromSolutionHeader = *static_cast<gendoc::parsing::ValueObject<gendoc::cppast::Solution*>*>(fromSolutionHeader_value.get());
            stack.pop();
        }
    }
    void PreProjects(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::cppast::Solution*>(context->value)));
    }
    void PreGlobal(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::cppast::Solution*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromSolutionHeader() {}
        gendoc::cppast::Solution* value;
        gendoc::cppast::Solution* fromSolutionHeader;
    };
};

class Solution::SolutionHeaderRule : public gendoc::parsing::Rule
{
public:
    SolutionHeaderRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::cppast::Solution*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::cppast::Solution*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SolutionHeaderRule>(this, &SolutionHeaderRule::A0Action));
        gendoc::parsing::NonterminalParser* formatVersionNonterminalParser = GetNonterminal(ToUtf32("FormatVersion"));
        formatVersionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SolutionHeaderRule>(this, &SolutionHeaderRule::PostFormatVersion));
        gendoc::parsing::NonterminalParser* visualStudionVersionNonterminalParser = GetNonterminal(ToUtf32("visualStudionVersion"));
        visualStudionVersionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SolutionHeaderRule>(this, &SolutionHeaderRule::PostvisualStudionVersion));
        gendoc::parsing::NonterminalParser* minVisualStudioVersionNonterminalParser = GetNonterminal(ToUtf32("minVisualStudioVersion"));
        minVisualStudioVersionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<SolutionHeaderRule>(this, &SolutionHeaderRule::PostminVisualStudioVersion));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new gendoc::cppast::Solution(fileName, context->fromFormatVersion, context->fromvisualStudionVersion, context->fromminVisualStudioVersion);
    }
    void PostFormatVersion(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFormatVersion_value = std::move(stack.top());
            context->fromFormatVersion = *static_cast<gendoc::parsing::ValueObject<Version>*>(fromFormatVersion_value.get());
            stack.pop();
        }
    }
    void PostvisualStudionVersion(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromvisualStudionVersion_value = std::move(stack.top());
            context->fromvisualStudionVersion = *static_cast<gendoc::parsing::ValueObject<Version>*>(fromvisualStudionVersion_value.get());
            stack.pop();
        }
    }
    void PostminVisualStudioVersion(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromminVisualStudioVersion_value = std::move(stack.top());
            context->fromminVisualStudioVersion = *static_cast<gendoc::parsing::ValueObject<Version>*>(fromminVisualStudioVersion_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromFormatVersion(), fromvisualStudionVersion(), fromminVisualStudioVersion() {}
        gendoc::cppast::Solution* value;
        Version fromFormatVersion;
        Version fromvisualStudionVersion;
        Version fromminVisualStudioVersion;
    };
};

class Solution::ProjectsRule : public gendoc::parsing::Rule
{
public:
    ProjectsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::cppast::Solution*"), ToUtf32("solution")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> solution_value = std::move(stack.top());
        context->solution = *static_cast<gendoc::parsing::ValueObject<gendoc::cppast::Solution*>*>(solution_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ProjectsRule>(this, &ProjectsRule::A0Action));
        gendoc::parsing::NonterminalParser* projectNonterminalParser = GetNonterminal(ToUtf32("Project"));
        projectNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectsRule>(this, &ProjectsRule::PostProject));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->solution->AddProject(context->fromProject);
    }
    void PostProject(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromProject_value = std::move(stack.top());
            context->fromProject = *static_cast<gendoc::parsing::ValueObject<Project*>*>(fromProject_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): solution(), fromProject() {}
        gendoc::cppast::Solution* solution;
        Project* fromProject;
    };
};

class Solution::ProjectRule : public gendoc::parsing::Rule
{
public:
    ProjectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Project*"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Project*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ProjectRule>(this, &ProjectRule::A0Action));
        gendoc::parsing::NonterminalParser* languageGuidNonterminalParser = GetNonterminal(ToUtf32("languageGuid"));
        languageGuidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::PostlanguageGuid));
        gendoc::parsing::NonterminalParser* projectNameNonterminalParser = GetNonterminal(ToUtf32("projectName"));
        projectNameNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::PostprojectName));
        gendoc::parsing::NonterminalParser* projectFilePathNonterminalParser = GetNonterminal(ToUtf32("projectFilePath"));
        projectFilePathNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::PostprojectFilePath));
        gendoc::parsing::NonterminalParser* projectGuidNonterminalParser = GetNonterminal(ToUtf32("projectGuid"));
        projectGuidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::PostprojectGuid));
        gendoc::parsing::NonterminalParser* projectContentNonterminalParser = GetNonterminal(ToUtf32("ProjectContent"));
        projectContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ProjectRule>(this, &ProjectRule::PreProjectContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Project(context->fromlanguageGuid, context->fromprojectName, context->fromprojectFilePath, context->fromprojectGuid);
    }
    void PostlanguageGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromlanguageGuid_value = std::move(stack.top());
            context->fromlanguageGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromlanguageGuid_value.get());
            stack.pop();
        }
    }
    void PostprojectName(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromprojectName_value = std::move(stack.top());
            context->fromprojectName = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromprojectName_value.get());
            stack.pop();
        }
    }
    void PostprojectFilePath(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromprojectFilePath_value = std::move(stack.top());
            context->fromprojectFilePath = *static_cast<gendoc::parsing::ValueObject<std::string>*>(fromprojectFilePath_value.get());
            stack.pop();
        }
    }
    void PostprojectGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromprojectGuid_value = std::move(stack.top());
            context->fromprojectGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromprojectGuid_value.get());
            stack.pop();
        }
    }
    void PreProjectContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Project*>(context->value)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromlanguageGuid(), fromprojectName(), fromprojectFilePath(), fromprojectGuid() {}
        Project* value;
        std::u32string fromlanguageGuid;
        std::u32string fromprojectName;
        std::string fromprojectFilePath;
        std::u32string fromprojectGuid;
    };
};

class Solution::ProjectContentRule : public gendoc::parsing::Rule
{
public:
    ProjectContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Project*"), ToUtf32("project")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> project_value = std::move(stack.top());
        context->project = *static_cast<gendoc::parsing::ValueObject<Project*>*>(project_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* projectSectionNonterminalParser = GetNonterminal(ToUtf32("ProjectSection"));
        projectSectionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ProjectContentRule>(this, &ProjectContentRule::PreProjectSection));
    }
    void PreProjectSection(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Project*>(context->project)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): project() {}
        Project* project;
    };
};

class Solution::ProjectSectionRule : public gendoc::parsing::Rule
{
public:
    ProjectSectionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Project*"), ToUtf32("project")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> project_value = std::move(stack.top());
        context->project = *static_cast<gendoc::parsing::ValueObject<Project*>*>(project_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* projectDependencySectionNonterminalParser = GetNonterminal(ToUtf32("ProjectDependencySection"));
        projectDependencySectionNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ProjectSectionRule>(this, &ProjectSectionRule::PreProjectDependencySection));
    }
    void PreProjectDependencySection(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Project*>(context->project)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): project() {}
        Project* project;
    };
};

class Solution::ProjectDependencySectionRule : public gendoc::parsing::Rule
{
public:
    ProjectDependencySectionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Project*"), ToUtf32("project")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> project_value = std::move(stack.top());
        context->project = *static_cast<gendoc::parsing::ValueObject<Project*>*>(project_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* projectDependencySectionContentNonterminalParser = GetNonterminal(ToUtf32("ProjectDependencySectionContent"));
        projectDependencySectionContentNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<ProjectDependencySectionRule>(this, &ProjectDependencySectionRule::PreProjectDependencySectionContent));
    }
    void PreProjectDependencySectionContent(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Project*>(context->project)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): project() {}
        Project* project;
    };
};

class Solution::ProjectDependencySectionContentRule : public gendoc::parsing::Rule
{
public:
    ProjectDependencySectionContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Project*"), ToUtf32("project")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> project_value = std::move(stack.top());
        context->project = *static_cast<gendoc::parsing::ValueObject<Project*>*>(project_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ProjectDependencySectionContentRule>(this, &ProjectDependencySectionContentRule::A0Action));
        gendoc::parsing::NonterminalParser* dependentProjectNonterminalParser = GetNonterminal(ToUtf32("DependentProject"));
        dependentProjectNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProjectDependencySectionContentRule>(this, &ProjectDependencySectionContentRule::PostDependentProject));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->project->AddDependency(context->fromDependentProject);
    }
    void PostDependentProject(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDependentProject_value = std::move(stack.top());
            context->fromDependentProject = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromDependentProject_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): project(), fromDependentProject() {}
        Project* project;
        std::u32string fromDependentProject;
    };
};

class Solution::DependentProjectRule : public gendoc::parsing::Rule
{
public:
    DependentProjectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DependentProjectRule>(this, &DependentProjectRule::A0Action));
        gendoc::parsing::NonterminalParser* projectGuidNonterminalParser = GetNonterminal(ToUtf32("projectGuid"));
        projectGuidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DependentProjectRule>(this, &DependentProjectRule::PostprojectGuid));
        gendoc::parsing::NonterminalParser* postGuidNonterminalParser = GetNonterminal(ToUtf32("postGuid"));
        postGuidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<DependentProjectRule>(this, &DependentProjectRule::PostpostGuid));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromprojectGuid;
    }
    void PostprojectGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromprojectGuid_value = std::move(stack.top());
            context->fromprojectGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromprojectGuid_value.get());
            stack.pop();
        }
    }
    void PostpostGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frompostGuid_value = std::move(stack.top());
            context->frompostGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(frompostGuid_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromprojectGuid(), frompostGuid() {}
        std::u32string value;
        std::u32string fromprojectGuid;
        std::u32string frompostGuid;
    };
};

class Solution::GlobalRule : public gendoc::parsing::Rule
{
public:
    GlobalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::cppast::Solution*"), ToUtf32("solution")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> solution_value = std::move(stack.top());
        context->solution = *static_cast<gendoc::parsing::ValueObject<gendoc::cppast::Solution*>*>(solution_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::NonterminalParser* globalSectionsNonterminalParser = GetNonterminal(ToUtf32("GlobalSections"));
        globalSectionsNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<GlobalRule>(this, &GlobalRule::PreGlobalSections));
    }
    void PreGlobalSections(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::cppast::Solution*>(context->solution)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): solution() {}
        gendoc::cppast::Solution* solution;
    };
};

class Solution::GlobalSectionsRule : public gendoc::parsing::Rule
{
public:
    GlobalSectionsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("gendoc::cppast::Solution*"), ToUtf32("solution")));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<gendoc::parsing::Object> solution_value = std::move(stack.top());
        context->solution = *static_cast<gendoc::parsing::ValueObject<gendoc::cppast::Solution*>*>(solution_value.get());
        stack.pop();
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): solution() {}
        gendoc::cppast::Solution* solution;
    };
};

class Solution::FormatVersionRule : public gendoc::parsing::Rule
{
public:
    FormatVersionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Version"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Version>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FormatVersionRule>(this, &FormatVersionRule::A0Action));
        gendoc::parsing::NonterminalParser* majorNonterminalParser = GetNonterminal(ToUtf32("major"));
        majorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FormatVersionRule>(this, &FormatVersionRule::Postmajor));
        gendoc::parsing::NonterminalParser* minorNonterminalParser = GetNonterminal(ToUtf32("minor"));
        minorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<FormatVersionRule>(this, &FormatVersionRule::Postminor));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Version(context->frommajor, context->fromminor, 0, 0);
    }
    void Postmajor(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frommajor_value = std::move(stack.top());
            context->frommajor = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(frommajor_value.get());
            stack.pop();
        }
    }
    void Postminor(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromminor_value = std::move(stack.top());
            context->fromminor = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromminor_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), frommajor(), fromminor() {}
        Version value;
        uint32_t frommajor;
        uint32_t fromminor;
    };
};

class Solution::ProgramVersionRule : public gendoc::parsing::Rule
{
public:
    ProgramVersionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Version"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<Version>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ProgramVersionRule>(this, &ProgramVersionRule::A0Action));
        gendoc::parsing::NonterminalParser* majorNonterminalParser = GetNonterminal(ToUtf32("major"));
        majorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProgramVersionRule>(this, &ProgramVersionRule::Postmajor));
        gendoc::parsing::NonterminalParser* minorNonterminalParser = GetNonterminal(ToUtf32("minor"));
        minorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProgramVersionRule>(this, &ProgramVersionRule::Postminor));
        gendoc::parsing::NonterminalParser* revisionNonterminalParser = GetNonterminal(ToUtf32("revision"));
        revisionNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProgramVersionRule>(this, &ProgramVersionRule::Postrevision));
        gendoc::parsing::NonterminalParser* buildNonterminalParser = GetNonterminal(ToUtf32("build"));
        buildNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<ProgramVersionRule>(this, &ProgramVersionRule::Postbuild));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Version(context->frommajor, context->fromminor, context->fromrevision, context->frombuild);
    }
    void Postmajor(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frommajor_value = std::move(stack.top());
            context->frommajor = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(frommajor_value.get());
            stack.pop();
        }
    }
    void Postminor(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromminor_value = std::move(stack.top());
            context->fromminor = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromminor_value.get());
            stack.pop();
        }
    }
    void Postrevision(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromrevision_value = std::move(stack.top());
            context->fromrevision = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromrevision_value.get());
            stack.pop();
        }
    }
    void Postbuild(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frombuild_value = std::move(stack.top());
            context->frombuild = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(frombuild_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), frommajor(), fromminor(), fromrevision(), frombuild() {}
        Version value;
        uint32_t frommajor;
        uint32_t fromminor;
        uint32_t fromrevision;
        uint32_t frombuild;
    };
};

class Solution::MajorVersionRule : public gendoc::parsing::Rule
{
public:
    MajorVersionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MajorVersionRule>(this, &MajorVersionRule::A0Action));
        gendoc::parsing::NonterminalParser* uintNonterminalParser = GetNonterminal(ToUtf32("uint"));
        uintNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MajorVersionRule>(this, &MajorVersionRule::Postuint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromuint;
    }
    void Postuint(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuint_value = std::move(stack.top());
            context->fromuint = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromuint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromuint() {}
        uint32_t value;
        uint32_t fromuint;
    };
};

class Solution::MinorVersionRule : public gendoc::parsing::Rule
{
public:
    MinorVersionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MinorVersionRule>(this, &MinorVersionRule::A0Action));
        gendoc::parsing::NonterminalParser* uintNonterminalParser = GetNonterminal(ToUtf32("uint"));
        uintNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<MinorVersionRule>(this, &MinorVersionRule::Postuint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromuint;
    }
    void Postuint(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuint_value = std::move(stack.top());
            context->fromuint = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromuint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromuint() {}
        uint32_t value;
        uint32_t fromuint;
    };
};

class Solution::RevisionRule : public gendoc::parsing::Rule
{
public:
    RevisionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RevisionRule>(this, &RevisionRule::A0Action));
        gendoc::parsing::NonterminalParser* uintNonterminalParser = GetNonterminal(ToUtf32("uint"));
        uintNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RevisionRule>(this, &RevisionRule::Postuint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromuint;
    }
    void Postuint(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuint_value = std::move(stack.top());
            context->fromuint = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromuint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromuint() {}
        uint32_t value;
        uint32_t fromuint;
    };
};

class Solution::BuildNumberRule : public gendoc::parsing::Rule
{
public:
    BuildNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BuildNumberRule>(this, &BuildNumberRule::A0Action));
        gendoc::parsing::NonterminalParser* uintNonterminalParser = GetNonterminal(ToUtf32("uint"));
        uintNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<BuildNumberRule>(this, &BuildNumberRule::Postuint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromuint;
    }
    void Postuint(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromuint_value = std::move(stack.top());
            context->fromuint = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromuint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromuint() {}
        uint32_t value;
        uint32_t fromuint;
    };
};

class Solution::GuidStringRule : public gendoc::parsing::Rule
{
public:
    GuidStringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GuidStringRule>(this, &GuidStringRule::A0Action));
        gendoc::parsing::NonterminalParser* guidNonterminalParser = GetNonterminal(ToUtf32("Guid"));
        guidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GuidStringRule>(this, &GuidStringRule::PostGuid));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGuid;
    }
    void PostGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGuid_value = std::move(stack.top());
            context->fromGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromGuid_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromGuid() {}
        std::u32string value;
        std::u32string fromGuid;
    };
};

class Solution::GuidRule : public gendoc::parsing::Rule
{
public:
    GuidRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GuidRule>(this, &GuidRule::A0Action));
        gendoc::parsing::NonterminalParser* hexGuidNonterminalParser = GetNonterminal(ToUtf32("HexGuid"));
        hexGuidNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<GuidRule>(this, &GuidRule::PostHexGuid));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexGuid;
    }
    void PostHexGuid(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexGuid_value = std::move(stack.top());
            context->fromHexGuid = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromHexGuid_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromHexGuid() {}
        std::u32string value;
        std::u32string fromHexGuid;
    };
};

class Solution::HexGuidRule : public gendoc::parsing::Rule
{
public:
    HexGuidRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HexGuidRule>(this, &HexGuidRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToLower(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Solution::FilePathRule : public gendoc::parsing::Rule
{
public:
    FilePathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
    }
    void Enter(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(gendoc::parsing::ObjectStack& stack, gendoc::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        std::string value;
    };
};

void Solution::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Solution::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("gendoc.parsing.stdlib.newline")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("gendoc.parsing.stdlib.string")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("gendoc.parsing.stdlib.uint")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("gendoc.parsing.stdlib.identifier")));
    AddRule(new SolutionRule(ToUtf32("Solution"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("SolutionHeader"), ToUtf32("SolutionHeader"), 0)),
                new gendoc::parsing::NonterminalParser(ToUtf32("Projects"), ToUtf32("Projects"), 1)),
            new gendoc::parsing::NonterminalParser(ToUtf32("Global"), ToUtf32("Global"), 1))));
    AddRule(new SolutionHeaderRule(ToUtf32("SolutionHeader"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::StringParser(ToUtf32("Microsoft Visual Studio Solution File")),
                                                new gendoc::parsing::CharParser(',')),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("FormatVersion"), ToUtf32("FormatVersion"), 0)),
                                        new gendoc::parsing::KeywordParser(ToUtf32("VisualStudioVersion"))),
                                    new gendoc::parsing::CharParser('=')),
                                new gendoc::parsing::NonterminalParser(ToUtf32("visualStudionVersion"), ToUtf32("ProgramVersion"), 0)),
                            new gendoc::parsing::KeywordParser(ToUtf32("MinimumVisualStudioVersion"))),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("minVisualStudioVersion"), ToUtf32("ProgramVersion"), 0))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::CharParser('#'),
                new gendoc::parsing::KleeneStarParser(
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::DifferenceParser(
                            new gendoc::parsing::AnyCharParser(),
                            new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))))),
            new gendoc::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SpacesOrComments"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SpaceParser(),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 0))))));
    AddRule(new ProjectsRule(ToUtf32("Projects"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("Project"), ToUtf32("Project"), 0))))));
    AddRule(new ProjectRule(ToUtf32("Project"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::GroupingParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::SequenceParser(
                                                            new gendoc::parsing::KeywordParser(ToUtf32("Project")),
                                                            new gendoc::parsing::CharParser('(')),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("languageGuid"), ToUtf32("GuidString"), 0)),
                                                    new gendoc::parsing::CharParser(')')),
                                                new gendoc::parsing::CharParser('=')),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("projectName"), ToUtf32("string"), 0)),
                                        new gendoc::parsing::CharParser(',')),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("projectFilePath"), ToUtf32("FilePath"), 0)),
                                new gendoc::parsing::CharParser(',')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("projectGuid"), ToUtf32("GuidString"), 0)))),
                new gendoc::parsing::NonterminalParser(ToUtf32("ProjectContent"), ToUtf32("ProjectContent"), 1)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndProject")))));
    AddRule(new ProjectContentRule(ToUtf32("ProjectContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("ProjectSection"), ToUtf32("ProjectSection"), 1))));
    AddRule(new ProjectSectionRule(ToUtf32("ProjectSection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::NonterminalParser(ToUtf32("ProjectDependencySection"), ToUtf32("ProjectDependencySection"), 1)));
    AddRule(new ProjectDependencySectionRule(ToUtf32("ProjectDependencySection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("ProjectSection")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::KeywordParser(ToUtf32("ProjectDependencies"))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::KeywordParser(ToUtf32("postProject"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("ProjectDependencySectionContent"), ToUtf32("ProjectDependencySectionContent"), 1)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndProjectSection")))));
    AddRule(new ProjectDependencySectionContentRule(ToUtf32("ProjectDependencySectionContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::ActionParser(ToUtf32("A0"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("DependentProject"), ToUtf32("DependentProject"), 0))))));
    AddRule(new DependentProjectRule(ToUtf32("DependentProject"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::NonterminalParser(ToUtf32("projectGuid"), ToUtf32("Guid"), 0),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("postGuid"), ToUtf32("Guid"), 0))))));
    AddRule(new GlobalRule(ToUtf32("Global"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("Global")),
                new gendoc::parsing::NonterminalParser(ToUtf32("GlobalSections"), ToUtf32("GlobalSections"), 1)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndGlobal")))));
    AddRule(new GlobalSectionsRule(ToUtf32("GlobalSections"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("GlobalSection"), ToUtf32("GlobalSection"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("GlobalSection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("SolutionConfigurationPlatformSection"), ToUtf32("SolutionConfigurationPlatformSection"), 0),
                    new gendoc::parsing::NonterminalParser(ToUtf32("ProjectConfigurationPlatformSection"), ToUtf32("ProjectConfigurationPlatformSection"), 0)),
                new gendoc::parsing::NonterminalParser(ToUtf32("SolutionPropertySection"), ToUtf32("SolutionPropertySection"), 0)),
            new gendoc::parsing::NonterminalParser(ToUtf32("ExtensibilityGlobalSection"), ToUtf32("ExtensibilityGlobalSection"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionConfigurationPlatformSection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("GlobalSection")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::KeywordParser(ToUtf32("SolutionConfigurationPlatforms"))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::KeywordParser(ToUtf32("preSolution"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("SolutionConfigurationPlatforms"), ToUtf32("SolutionConfigurationPlatforms"), 0)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndGlobalSection")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionConfigurationPlatforms"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("SolutionConfigurationPlatform"), ToUtf32("SolutionConfigurationPlatform"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionConfigurationPlatform"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("Configuration"), ToUtf32("Configuration"), 0),
                new gendoc::parsing::CharParser('=')),
            new gendoc::parsing::NonterminalParser(ToUtf32("Configuration"), ToUtf32("Configuration"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Configuration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("identifier"), 0),
                    new gendoc::parsing::CharParser('|')),
                new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Platform"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("Platform"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::StringParser(ToUtf32("Any CPU")),
            new gendoc::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ProjectConfigurationPlatformSection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("GlobalSection")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::KeywordParser(ToUtf32("ProjectConfigurationPlatforms"))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::KeywordParser(ToUtf32("postSolution"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("ProjectConfigurationPlatforms"), ToUtf32("ProjectConfigurationPlatforms"), 0)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndGlobalSection")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ProjectConfigurationPlatforms"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("ProjectConfigurationPlatform"), ToUtf32("ProjectConfigurationPlatform"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ProjectConfigurationPlatform"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("ProjectConfiguration"), ToUtf32("ProjectConfiguration"), 0),
                new gendoc::parsing::CharParser('=')),
            new gendoc::parsing::NonterminalParser(ToUtf32("Configuration"), ToUtf32("Configuration"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ProjectConfiguration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("Guid"), ToUtf32("Guid"), 0),
                            new gendoc::parsing::CharParser('.')),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Configuration"), ToUtf32("Configuration"), 0)),
                    new gendoc::parsing::CharParser('.')),
                new gendoc::parsing::NonterminalParser(ToUtf32("ConfigName"), ToUtf32("ConfigName"), 0)))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ConfigName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::PositiveParser(
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::LetterParser(),
                        new gendoc::parsing::DigitParser()),
                    new gendoc::parsing::CharParser('.'))))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionPropertySection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("GlobalSection")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::KeywordParser(ToUtf32("SolutionProperties"))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::KeywordParser(ToUtf32("preSolution"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("SolutionProperties"), ToUtf32("SolutionProperties"), 0)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndGlobalSection")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionProperties"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("SolutionProperty"), ToUtf32("SolutionProperty"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("SolutionProperty"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("identifier"), 0),
                new gendoc::parsing::CharParser('=')),
            new gendoc::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("identifier"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExtensibilityGlobalSection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("GlobalSection")),
                                    new gendoc::parsing::CharParser('(')),
                                new gendoc::parsing::KeywordParser(ToUtf32("ExtensibilityGlobals"))),
                            new gendoc::parsing::CharParser(')')),
                        new gendoc::parsing::CharParser('=')),
                    new gendoc::parsing::KeywordParser(ToUtf32("postSolution"))),
                new gendoc::parsing::NonterminalParser(ToUtf32("ExtensibilityGlobals"), ToUtf32("ExtensibilityGlobals"), 0)),
            new gendoc::parsing::KeywordParser(ToUtf32("EndGlobalSection")))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExtensibilityGlobals"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::KleeneStarParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("ExtensibilityGlobal"), ToUtf32("ExtensibilityGlobal"), 0))));
    AddRule(new gendoc::parsing::Rule(ToUtf32("ExtensibilityGlobal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::KeywordParser(ToUtf32("SolutionGuid")),
                new gendoc::parsing::CharParser('=')),
            new gendoc::parsing::NonterminalParser(ToUtf32("Guid"), ToUtf32("Guid"), 0))));
    AddRule(new FormatVersionRule(ToUtf32("FormatVersion"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::StringParser(ToUtf32("Format Version ")),
                            new gendoc::parsing::NonterminalParser(ToUtf32("major"), ToUtf32("MajorVersion"), 0)),
                        new gendoc::parsing::CharParser('.')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("minor"), ToUtf32("MinorVersion"), 0))))));
    AddRule(new ProgramVersionRule(ToUtf32("ProgramVersion"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::NonterminalParser(ToUtf32("major"), ToUtf32("MajorVersion"), 0),
                                        new gendoc::parsing::CharParser('.')),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("minor"), ToUtf32("MinorVersion"), 0)),
                                new gendoc::parsing::CharParser('.')),
                            new gendoc::parsing::NonterminalParser(ToUtf32("revision"), ToUtf32("Revision"), 0)),
                        new gendoc::parsing::CharParser('.')),
                    new gendoc::parsing::NonterminalParser(ToUtf32("build"), ToUtf32("BuildNumber"), 0))))));
    AddRule(new MajorVersionRule(ToUtf32("MajorVersion"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("uint"), ToUtf32("uint"), 0))));
    AddRule(new MinorVersionRule(ToUtf32("MinorVersion"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("uint"), ToUtf32("uint"), 0))));
    AddRule(new RevisionRule(ToUtf32("Revision"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("uint"), ToUtf32("uint"), 0))));
    AddRule(new BuildNumberRule(ToUtf32("BuildNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("uint"), ToUtf32("uint"), 0))));
    AddRule(new GuidStringRule(ToUtf32("GuidString"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('\"'),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Guid"), ToUtf32("Guid"), 0)),
                    new gendoc::parsing::CharParser('\"'))))));
    AddRule(new GuidRule(ToUtf32("Guid"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('{'),
                        new gendoc::parsing::NonterminalParser(ToUtf32("HexGuid"), ToUtf32("HexGuid"), 0)),
                    new gendoc::parsing::CharParser('}'))))));
    AddRule(new HexGuidRule(ToUtf32("HexGuid"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::SequenceParser(
                                        new gendoc::parsing::SequenceParser(
                                            new gendoc::parsing::SequenceParser(
                                                new gendoc::parsing::SequenceParser(
                                                    new gendoc::parsing::SequenceParser(
                                                        new gendoc::parsing::SequenceParser(
                                                            new gendoc::parsing::SequenceParser(
                                                                new gendoc::parsing::SequenceParser(
                                                                    new gendoc::parsing::SequenceParser(
                                                                        new gendoc::parsing::SequenceParser(
                                                                            new gendoc::parsing::SequenceParser(
                                                                                new gendoc::parsing::SequenceParser(
                                                                                    new gendoc::parsing::SequenceParser(
                                                                                        new gendoc::parsing::SequenceParser(
                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                    new gendoc::parsing::SequenceParser(
                                                                                                        new gendoc::parsing::SequenceParser(
                                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                                    new gendoc::parsing::SequenceParser(
                                                                                                                        new gendoc::parsing::SequenceParser(
                                                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                                                    new gendoc::parsing::SequenceParser(
                                                                                                                                        new gendoc::parsing::SequenceParser(
                                                                                                                                            new gendoc::parsing::SequenceParser(
                                                                                                                                                new gendoc::parsing::SequenceParser(
                                                                                                                                                    new gendoc::parsing::SequenceParser(
                                                                                                                                                        new gendoc::parsing::SequenceParser(
                                                                                                                                                            new gendoc::parsing::HexDigitParser(),
                                                                                                                                                            new gendoc::parsing::HexDigitParser()),
                                                                                                                                                        new gendoc::parsing::HexDigitParser()),
                                                                                                                                                    new gendoc::parsing::HexDigitParser()),
                                                                                                                                                new gendoc::parsing::HexDigitParser()),
                                                                                                                                            new gendoc::parsing::HexDigitParser()),
                                                                                                                                        new gendoc::parsing::HexDigitParser()),
                                                                                                                                    new gendoc::parsing::HexDigitParser()),
                                                                                                                                new gendoc::parsing::CharParser('-')),
                                                                                                                            new gendoc::parsing::HexDigitParser()),
                                                                                                                        new gendoc::parsing::HexDigitParser()),
                                                                                                                    new gendoc::parsing::HexDigitParser()),
                                                                                                                new gendoc::parsing::HexDigitParser()),
                                                                                                            new gendoc::parsing::CharParser('-')),
                                                                                                        new gendoc::parsing::HexDigitParser()),
                                                                                                    new gendoc::parsing::HexDigitParser()),
                                                                                                new gendoc::parsing::HexDigitParser()),
                                                                                            new gendoc::parsing::HexDigitParser()),
                                                                                        new gendoc::parsing::CharParser('-')),
                                                                                    new gendoc::parsing::HexDigitParser()),
                                                                                new gendoc::parsing::HexDigitParser()),
                                                                            new gendoc::parsing::HexDigitParser()),
                                                                        new gendoc::parsing::HexDigitParser()),
                                                                    new gendoc::parsing::CharParser('-')),
                                                                new gendoc::parsing::HexDigitParser()),
                                                            new gendoc::parsing::HexDigitParser()),
                                                        new gendoc::parsing::HexDigitParser()),
                                                    new gendoc::parsing::HexDigitParser()),
                                                new gendoc::parsing::HexDigitParser()),
                                            new gendoc::parsing::HexDigitParser()),
                                        new gendoc::parsing::HexDigitParser()),
                                    new gendoc::parsing::HexDigitParser()),
                                new gendoc::parsing::HexDigitParser()),
                            new gendoc::parsing::HexDigitParser()),
                        new gendoc::parsing::HexDigitParser()),
                    new gendoc::parsing::HexDigitParser())))));
    AddRule(new FilePathRule(ToUtf32("FilePath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::TokenParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('\"'),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::GroupingParser(
                            new gendoc::parsing::PositiveParser(
                                new gendoc::parsing::CharSetParser(ToUtf32("\""), true))))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser('\"'))))));
    SetSkipRuleName(ToUtf32("SpacesOrComments"));
}

} } // namespace gendoc.cppparser
