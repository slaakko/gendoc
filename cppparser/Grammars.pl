namespace gendoc.cppparser
{
    grammar Class
    {
        ClassDeclaration(ParsingContext* ctx): ClassNode*;
        ForwardClassDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> cn): Node*;
        ClassSpecifier(ParsingContext* ctx, var std::unique_ptr<ClassNode> classNode): ClassNode*;
        MemberSpecifications(ParsingContext* ctx, ClassNode* classNode);
        MemberSpecification(ParsingContext* ctx, ClassNode* classNode, var Span s);
        MemberDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var std::unique_ptr<Node> declarator, var std::unique_ptr<Node> initializer): Node*;
        SpecialMemberFunctionDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> declarator): Node*;
        ClassHead(ParsingContext* ctx, var std::unique_ptr<Node> cn): ClassNode*;
        ClassKey: ClassKey;
        ClassName(ParsingContext* ctx): Node*;
        ClassVirtSpecifiers: Specifier;
        ClassVirtSpecifier: Specifier;
        BaseClause(ParsingContext* ctx): Node*;
        BaseClassSpecifierList(ParsingContext* ctx, var std::unique_ptr<Node> node, var Span s): Node*;
        BaseClassSpecifier(ParsingContext* ctx): BaseClassSpecifierNode*;
        BaseSpecifiers: Specifier;
        BaseSpecifier: Specifier;
        AccessSpecifier: Specifier;
        VirtPureSpecifiers(bool functionMember): Specifier;
        VirtSpecifier(bool functionMember): Specifier;
        PureSpecifier(bool functionMember): Specifier;
        SpecialMemberFunctionDefinition(ParsingContext* ctx, var std::unique_ptr<Node> declarator, var std::unique_ptr<Node> ctorInitializer, var Specifier defaultOrDeleteSpecifier): Node*;
        CtorInitializer(ParsingContext* ctx): Node*;
        MemberInitializerList(ParsingContext* ctx, var std::unique_ptr<Node> list): Node*;
        MemberInitializer(ParsingContext* ctx, var std::unique_ptr<Node> id, var Span exprListSpan): MemberInitializerNode*;
        MemberInitializerId(ParsingContext* ctx): Node*;
    }
    grammar Declaration
    {
        Declaration(ParsingContext* ctx): Node*;
        NamespaceDefinition(ParsingContext* ctx): Node*;
        NamedNamespaceDefinition(ParsingContext* ctx, var std::unique_ptr<NamespaceNode> ns): NamespaceNode*;
        UnnamedNamespaceDefinition(ParsingContext* ctx, var std::unique_ptr<NamespaceNode> ns): NamespaceNode*;
        BlockDeclaration(ParsingContext* ctx): Node*;
        SimpleDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var std::unique_ptr<Node> initDeclarator): Node*;
        DeclSpecifiers: Specifier;
        DeclSpecifier: Specifier;
        StorageClassSpecifier: Specifier;
        FunctionSpecifier: Specifier;
        AliasDeclaration(ParsingContext* ctx): AliasDeclarationNode*;
        UsingDirective(ParsingContext* ctx): UsingDirectiveNode*;
        UsingDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> qid): UsingDeclarationNode*;
        TypedefDeclaration(ParsingContext* ctx): TypedefNode*;
        LinkageSpecification(ParsingContext* ctx, var std::unique_ptr<LinkageSpecificationNode> ls): LinkageSpecificationNode*;
    }
    grammar CppToken
    {
        SourceLine(SourceCodeWriter* writer);
        SourceToken(SourceCodeWriter* writer);
        LineComment;
        BlockComment;
        PPLine;
        Space;
    }
    grammar CppSourceFile
    {
        SourceFile(SourceFileNode* sourceFile, var ParsingContext ctx);
        Declarations(NamespaceNode* ns, ParsingContext* ctx);
        CommentsAndSpacesAndPPLines;
        PPLine;
        S;
    }
    grammar Declarator
    {
        InitDeclarator(ParsingContext* ctx, var std::unique_ptr<Node> declarator, var Span s): Node*;
        Declarator(ParsingContext* ctx, var std::unique_ptr<Node> node, var Span s): Node*;
        Initializer(ParsingContext* ctx): Node*;
        BraceOrEqualInitializer(ParsingContext* ctx): Node*;
        BracedInitializerList(ParsingContext* ctx, var std::unique_ptr<BracedInitializerListNode> node, var Span s): BracedInitializerListNode*;
        InitializerClause(ParsingContext* ctx): Node*;
        InitializerList(ParsingContext* ctx, var std::unique_ptr<Node> node, var Span s): Node*;
    }
    grammar Enumeration
    {
        EnumDeclaration(ParsingContext* ctx): EnumTypeNode*;
        OpaqueEnumDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> enumName, var std::unique_ptr<Node> enumBase): Node*;
        EnumSpecifier(ParsingContext* ctx, var std::unique_ptr<EnumTypeNode> enumTypeNode, var Span openBraceSpan, var Span closeBraceSpan): EnumTypeNode*;
        EnumHead(ParsingContext* ctx, var std::unique_ptr<Node> enumName): EnumTypeNode*;
        EnumKey: EnumKey;
        EnumName(ParsingContext* ctx): Node*;
        EnumBase(ParsingContext* ctx): Node*;
        Enumerators(EnumTypeNode* enumTypeNode, ParsingContext* ctx, var Span s);
        EnumeratorDefinition(ParsingContext* ctx, var Span assignmentSignSpan, var std::u32string valueStr): EnumeratorNode*;
        Enumerator: std::u32string;
    }
    grammar Expression
    {
        ExpressionList(ParsingContext* ctx): Node*;
        PossiblyEmptyArgumentList(ParsingContext* ctx): Node*;
        Expression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        ConstantExpression(ParsingContext* ctx): Node*;
        AssignmentExpression(ParsingContext* ctx): Node*;
        ConcreteAssignmentExpression(ParsingContext* ctx, var std::unique_ptr<Node> logicalOrExpr): Node*;
        AssignmentOperator: Operator;
        ConditionalExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var std::unique_ptr<Node> qe, var Span s): Node*;
        ThrowExpression(ParsingContext* ctx): Node*;
        LogicalOrExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        LogicalAndExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        InclusiveOrExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        ExclusiveOrExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        AndExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        EqualityExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        RelationalExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        ShiftExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        AdditiveExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        MultiplicativeExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        PMExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        CastExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr): Node*;
        UnaryExpression(ParsingContext* ctx): Node*;
        UnaryOperator: Operator;
        NewExpression(ParsingContext* ctx): Node*;
        NewPlacement(ParsingContext* ctx): Node*;
        NewInitializer(ParsingContext* ctx): Node*;
        DeleteExpression(ParsingContext* ctx, var bool array_): Node*;
        PostfixExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr, var std::unique_ptr<Node> args, var Span s): Node*;
        PrimaryExpression(ParsingContext* ctx, var std::unique_ptr<Node> expr): Node*;
        CppCastExpression(ParsingContext* ctx, var Operator op): Node*;
        TypeIdExpression(ParsingContext* ctx): Node*;
        IdExpression(ParsingContext* ctx): Node*;
        UnqualifiedId(ParsingContext* ctx): Node*;
        QualifiedId(ParsingContext* ctx, var std::unique_ptr<Node> node): Node*;
        OperatorFunctionId: Node*;
        Operator: Operator;
        ConversionFunctionId(ParsingContext* ctx): Node*;
    }
    grammar Function
    {
        FunctionDefinition(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var std::unique_ptr<Node> declarator, var Specifier defaultOrDeleteSpecifier): Node*;
        ParameterList(ParsingContext* ctx, var std::unique_ptr<Node> parameter, var Span s): Node*;
        ParameterDeclaration(ParsingContext* ctx): ParameterNode*;
        FunctionBody(ParsingContext* ctx): CompoundStatementNode*;
    }
    grammar Identifier
    {
        Identifier: std::u32string;
        IdNode(ParsingContext* ctx): Node*;
        NestedNameSpecifier(ParsingContext* ctx, var std::unique_ptr<Node> node): Node*;
        QualifiedIdNode(ParsingContext* ctx, var std::unique_ptr<Node> node): Node*;
    }
    grammar Literal
    {
        Literal: LiteralNode*;
        FloatingLiteral(var std::string m, var Suffix suffix): FloatingLiteralNode*;
        FractionalConstant;
        ExponentPart;
        FloatingSuffix: Suffix;
        IntegerLiteral(var uint64_t x, var Suffix suffix, var Base base): IntegerLiteralNode*;
        DecimalLiteral: uint64_t;
        OctalLiteral: uint64_t;
        HexadecimalLiteral: uint64_t;
        IntegerSuffix: Suffix;
        CharacterLiteral(var char32_t prefix, var std::u32string chars): CharacterLiteralNode*;
        CChar: char32_t;
        HexDigit4: uint16_t;
        HexDigit8: uint32_t;
        StringLiteral(var std::u32string encodingPrefix, var std::u32string chars): StringLiteralNode*;
        EncodingPrefix: std::u32string;
        SChar: char32_t;
        BooleanLiteral: LiteralNode*;
        PointerLiteral: LiteralNode*;
    }
    grammar IncludeDirective
    {
        SourceFile(SourceFileNode* sourceFileNode);
        SourceLine(SourceFileNode* sourceFile);
        IncludeDirectiveLine(SourceFileNode* sourceFile);
        OtherLine;
        FilePath: std::string;
        S;
    }
    grammar Keyword
    {
        Keyword;
    }
    grammar ParserToken
    {
        SourceLine(SourceCodeWriter* writer);
        SourceToken(SourceCodeWriter* writer);
        LineComment;
        BlockComment;
        PPLine;
        CharSet(var bool inverse, var std::u32string s);
        CharSetRange: std::u32string;
        CharSetChar: char32_t;
        Space;
    }
    grammar SimpleType
    {
        SimpleType(var std::vector<SimpleTypeSpecifier> specifiers): SimpleTypeNode*;
        SimpleTypeSpecifier: SimpleTypeSpecifier;
    }
    grammar Solution
    {
        Solution: gendoc::cppast::Solution*;
        SolutionHeader: gendoc::cppast::Solution*;
        Comment;
        SpacesOrComments;
        Projects(gendoc::cppast::Solution* solution);
        Project: Project*;
        ProjectContent(Project* project);
        ProjectSection(Project* project);
        ProjectDependencySection(Project* project);
        ProjectDependencySectionContent(Project* project);
        DependentProject: std::u32string;
        Global(gendoc::cppast::Solution* solution);
        GlobalSections(gendoc::cppast::Solution* solution);
        GlobalSection;
        SolutionConfigurationPlatformSection;
        SolutionConfigurationPlatforms;
        SolutionConfigurationPlatform;
        Configuration;
        Platform;
        ProjectConfigurationPlatformSection;
        ProjectConfigurationPlatforms;
        ProjectConfigurationPlatform;
        ProjectConfiguration;
        ConfigName;
        SolutionPropertySection;
        SolutionProperties;
        SolutionProperty;
        ExtensibilityGlobalSection;
        ExtensibilityGlobals;
        ExtensibilityGlobal;
        FormatVersion: Version;
        ProgramVersion: Version;
        MajorVersion: uint32_t;
        MinorVersion: uint32_t;
        Revision: uint32_t;
        BuildNumber: uint32_t;
        GuidString: std::u32string;
        Guid: std::u32string;
        HexGuid: std::u32string;
        FilePath: std::string;
    }
    grammar SourceLines
    {
        SourceFile: int;
        SourceLine: int;
        PPLine;
        S;
    }
    grammar Statement
    {
        Statement(ParsingContext* ctx): Node*;
        LabeledStatement(ParsingContext* ctx): Node*;
        ExpressionStatement(ParsingContext* ctx): Node*;
        CompoundStatement(ParsingContext* ctx, var std::unique_ptr<CompoundStatementNode> node): CompoundStatementNode*;
        SelectionStatement(ParsingContext* ctx, var Span elseSpan): Node*;
        Condition(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var std::unique_ptr<Node> declarator, var Span s): Node*;
        IterationStatement(ParsingContext* ctx, var std::unique_ptr<Node> forRange, var Span whileSpan): Node*;
        ForInitStatement(ParsingContext* ctx): Node*;
        ForRangeDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): Node*;
        ForRangeInitializer(ParsingContext* ctx): Node*;
        JumpStatement(ParsingContext* ctx): Node*;
        DeclarationStatement(ParsingContext* ctx): Node*;
        TryBlock(ParsingContext* ctx): TryStatementNode*;
        HandlerSequence(TryStatementNode* tryStatementNode, ParsingContext* ctx);
        Handler(ParsingContext* ctx): Node*;
        ExceptionDeclaration(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): Node*;
    }
    grammar Template
    {
        TemplateDeclaration(ParsingContext* ctx): Node*;
        TemplateParameterList(ParsingContext* ctx, var std::unique_ptr<Node> node, var Span s): Node*;
        TemplateParameter(ParsingContext* ctx): Node*;
        TypeParameter(ParsingContext* ctx, var bool typenameUsed): Node*;
        SimpleTemplateId(ParsingContext* ctx, var std::unique_ptr<Node> args, var int arity): Node*;
        TemplateId(ParsingContext* ctx): Node*;
        TemplateName: IdentifierNode*;
        TemplateArgumentList(ParsingContext* ctx, int* arity, var std::unique_ptr<Node> arg): Node*;
        TemplateArgument(ParsingContext* ctx): Node*;
        ExplicitInstantiation(ParsingContext* ctx): Node*;
        ExplicitSpecialization(ParsingContext* ctx): Node*;
    }
    grammar TypeExpr
    {
        TypeExpr(ParsingContext* ctx): Node*;
        PrefixTypeExpr(ParsingContext* ctx): Node*;
        CVSpecifierSequence: Specifier;
        CVSpecifier: Specifier;
        PostfixTypeExpr(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var Span s): Node*;
        PrimaryTypeExpr(ParsingContext* ctx, var ClassKey key): Node*;
    }
}
