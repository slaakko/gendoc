namespace gendoc.syntax
{
    grammar Composite
    {
        Alternative(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Sequence(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Difference(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        ExclusiveOr(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Intersection(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        List(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Postfix(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
    }
    grammar Rule
    {
        Rule(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Rule*;
        RuleHeader(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Rule*;
        RuleBody(gendoc::parsing::Rule* rule);
    }
    grammar Element
    {
        RuleLink(gendoc::parsing::Grammar* grammar);
        Signature(gendoc::parsing::Rule* rule);
        ParameterList(gendoc::parsing::Rule* rule);
        Variable(gendoc::parsing::Rule* rule);
        Parameter(gendoc::parsing::Rule* rule);
        ReturnType(gendoc::parsing::Rule* rule);
        Keyword;
        Identifier: std::u32string;
        QualifiedId: std::u32string;
        StringArray(std::vector<std::u32string>* array);
    }
    grammar GrammarGrammar
    {
        Grammar(gendoc::parsing::Scope* enclosingScope, ParsingDomain* parsingDomain): gendoc::parsing::Grammar*;
        GrammarContent(gendoc::parsing::Grammar* grammar);
        StartClause(gendoc::parsing::Grammar* grammar);
        SkipClause(gendoc::parsing::Grammar* grammar);
    }
    grammar Primitive
    {
        Primitive: gendoc::parsing::Parser*;
        Char: gendoc::parsing::Parser*;
        String: gendoc::parsing::Parser*;
        CharSet(var bool inverse, var std::u32string s): gendoc::parsing::Parser*;
        CharSetRange: std::u32string;
        CharSetChar: char32_t;
        Keyword: gendoc::parsing::Parser*;
        KeywordBody: gendoc::parsing::Parser*;
        KeywordList: gendoc::parsing::Parser*;
        KeywordListBody(var std::vector<std::u32string> keywords): gendoc::parsing::Parser*;
        Range: gendoc::parsing::Parser*;
        CodePoint: uint32_t;
        Empty: gendoc::parsing::Parser*;
        Space: gendoc::parsing::Parser*;
        AnyChar: gendoc::parsing::Parser*;
        Letter: gendoc::parsing::Parser*;
        Digit: gendoc::parsing::Parser*;
        HexDigit: gendoc::parsing::Parser*;
        UpperLetter: gendoc::parsing::Parser*;
        LowerLetter: gendoc::parsing::Parser*;
        TitleLetter: gendoc::parsing::Parser*;
        ModifierLetter: gendoc::parsing::Parser*;
        OtherLetter: gendoc::parsing::Parser*;
        CasedLetter: gendoc::parsing::Parser*;
        Mark: gendoc::parsing::Parser*;
        NonspacingMark: gendoc::parsing::Parser*;
        SpacingMark: gendoc::parsing::Parser*;
        EnclosingMark: gendoc::parsing::Parser*;
        Number: gendoc::parsing::Parser*;
        DecimalNumber: gendoc::parsing::Parser*;
        LetterNumber: gendoc::parsing::Parser*;
        OtherNumber: gendoc::parsing::Parser*;
        Punctuation: gendoc::parsing::Parser*;
        ConnectorPunctuation: gendoc::parsing::Parser*;
        DashPunctuation: gendoc::parsing::Parser*;
        OpenPunctuation: gendoc::parsing::Parser*;
        ClosePunctuation: gendoc::parsing::Parser*;
        InitialPunctuation: gendoc::parsing::Parser*;
        FinalPunctuation: gendoc::parsing::Parser*;
        OtherPunctuation: gendoc::parsing::Parser*;
        Symbol: gendoc::parsing::Parser*;
        MathSymbol: gendoc::parsing::Parser*;
        CurrencySymbol: gendoc::parsing::Parser*;
        ModifierSymbol: gendoc::parsing::Parser*;
        OtherSymbol: gendoc::parsing::Parser*;
        Separator: gendoc::parsing::Parser*;
        SpaceSeparator: gendoc::parsing::Parser*;
        LineSeparator: gendoc::parsing::Parser*;
        ParagraphSeparator: gendoc::parsing::Parser*;
        Other: gendoc::parsing::Parser*;
        Control: gendoc::parsing::Parser*;
        Format: gendoc::parsing::Parser*;
        Surrogate: gendoc::parsing::Parser*;
        PrivateUse: gendoc::parsing::Parser*;
        Unassigned: gendoc::parsing::Parser*;
        Graphic: gendoc::parsing::Parser*;
        BaseChar: gendoc::parsing::Parser*;
        Alphabetic: gendoc::parsing::Parser*;
        IdStart: gendoc::parsing::Parser*;
        IdCont: gendoc::parsing::Parser*;
    }
    grammar ParserFile
    {
        ParserFile(int id_, gendoc::parsing::ParsingDomain* parsingDomain_): ParserFileContent*;
        IncludeDirectives(ParserFileContent* parserFileContent);
        IncludeDirective(ParserFileContent* parserFileContent);
        FileAttribute: std::u32string;
        IncludeFileName: std::u32string;
        NamespaceContent(ParserFileContent* parserFileContent, ParsingDomain* parsingDomain);
        Namespace(ParserFileContent* parserFileContent, ParsingDomain* parsingDomain);
    }
    grammar LibraryFile
    {
        LibraryFile(gendoc::parsing::ParsingDomain* parsingDomain);
        NamespaceContent(gendoc::parsing::ParsingDomain* parsingDomain);
        Namespace(gendoc::parsing::ParsingDomain* parsingDomain);
        Grammar(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Grammar*;
        GrammarContent(gendoc::parsing::Grammar* grammar);
        Rule(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Rule*;
    }
    grammar Primary
    {
        Primary(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        RuleCall: gendoc::parsing::Parser*;
        Nonterminal: std::u32string;
        Alias: std::u32string;
        Grouping(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Token(gendoc::parsing::Scope* enclosingScope): gendoc::parsing::Parser*;
        Expectation(gendoc::parsing::Parser* child): gendoc::parsing::Parser*;
        Action(gendoc::parsing::Parser* child): gendoc::parsing::Parser*;
    }
    grammar ProjectFile
    {
        ProjectFile: Project*;
        ProjectFileContent(Project* project);
        Source: std::u32string;
        Reference: std::u32string;
        FilePath: std::u32string;
    }
}
