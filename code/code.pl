namespace gendoc.code
{
    grammar Declaration
    {
        BlockDeclaration: gendoc::codedom::CppObject*;
        SimpleDeclaration(var std::unique_ptr<SimpleDeclaration> sd): gendoc::codedom::SimpleDeclaration*;
        DeclSpecifierSeq(gendoc::codedom::SimpleDeclaration* declaration);
        DeclSpecifier: gendoc::codedom::DeclSpecifier*;
        StorageClassSpecifier: gendoc::codedom::StorageClassSpecifier*;
        TypeSpecifier: gendoc::codedom::TypeSpecifier*;
        SimpleTypeSpecifier: gendoc::codedom::TypeSpecifier*;
        TypeName: gendoc::codedom::TypeName*;
        TemplateArgumentList(gendoc::codedom::TypeName* typeName);
        TemplateArgument: gendoc::codedom::CppObject*;
        Typedef: gendoc::codedom::DeclSpecifier*;
        CVQualifier: gendoc::codedom::TypeSpecifier*;
        NamespaceAliasDefinition: gendoc::codedom::UsingObject*;
        UsingDeclaration: gendoc::codedom::UsingObject*;
        UsingDirective: gendoc::codedom::UsingObject*;
    }
    grammar Statement
    {
        Statement: gendoc::codedom::Statement*;
        LabeledStatement: gendoc::codedom::Statement*;
        Label: std::u32string;
        EmptyStatement: gendoc::codedom::Statement*;
        ExpressionStatement: gendoc::codedom::Statement*;
        CompoundStatement(var std::unique_ptr<CompoundStatement> cs): gendoc::codedom::CompoundStatement*;
        SelectionStatement: gendoc::codedom::Statement*;
        IfStatement: gendoc::codedom::Statement*;
        SwitchStatement: gendoc::codedom::Statement*;
        IterationStatement: gendoc::codedom::Statement*;
        WhileStatement: gendoc::codedom::Statement*;
        DoStatement: gendoc::codedom::Statement*;
        ForStatement: gendoc::codedom::Statement*;
        ForInitStatement: gendoc::codedom::CppObject*;
        JumpStatement: gendoc::codedom::Statement*;
        BreakStatement: gendoc::codedom::Statement*;
        ContinueStatement: gendoc::codedom::Statement*;
        ReturnStatement: gendoc::codedom::Statement*;
        GotoStatement: gendoc::codedom::Statement*;
        GotoTarget: std::u32string;
        DeclarationStatement: gendoc::codedom::Statement*;
        Condition(var std::unique_ptr<TypeId> ti): gendoc::codedom::CppObject*;
        TryStatement: gendoc::codedom::TryStatement*;
        HandlerSeq(TryStatement* st);
        Handler: gendoc::codedom::Handler*;
        ExceptionDeclaration(var std::unique_ptr<ExceptionDeclaration> ed): gendoc::codedom::ExceptionDeclaration*;
    }
    grammar Declarator
    {
        InitDeclaratorList(var std::unique_ptr<InitDeclaratorList> idl): gendoc::codedom::InitDeclaratorList*;
        InitDeclarator: gendoc::codedom::InitDeclarator*;
        Declarator: std::u32string;
        DirectDeclarator(var std::unique_ptr<CppObject> o);
        DeclaratorId(var std::unique_ptr<CppObject> o);
        TypeId(var std::unique_ptr<TypeId> ti): gendoc::codedom::TypeId*;
        Type(var std::unique_ptr<Type> t): gendoc::codedom::Type*;
        TypeSpecifierSeq(gendoc::codedom::TypeId* typeId);
        AbstractDeclarator: std::u32string;
        DirectAbstractDeclarator(var std::unique_ptr<CppObject> o);
        PtrOperator;
        CVQualifierSeq(var std::unique_ptr<CppObject> o);
        Initializer: gendoc::codedom::Initializer*;
        InitializerClause: gendoc::codedom::AssignInit*;
        InitializerList(gendoc::codedom::AssignInit* init);
    }
    grammar Expression
    {
        Expression: gendoc::codedom::CppObject*;
        ConstantExpression: gendoc::codedom::CppObject*;
        AssignmentExpression(var std::unique_ptr<CppObject> lor): gendoc::codedom::CppObject*;
        AssingmentOp: Operator;
        ThrowExpression: gendoc::codedom::CppObject*;
        ConditionalExpression: gendoc::codedom::CppObject*;
        LogicalOrExpression: gendoc::codedom::CppObject*;
        LogicalAndExpression: gendoc::codedom::CppObject*;
        InclusiveOrExpression: gendoc::codedom::CppObject*;
        ExclusiveOrExpression: gendoc::codedom::CppObject*;
        AndExpression: gendoc::codedom::CppObject*;
        EqualityExpression: gendoc::codedom::CppObject*;
        EqOp: Operator;
        RelationalExpression: gendoc::codedom::CppObject*;
        RelOp: Operator;
        ShiftExpression: gendoc::codedom::CppObject*;
        ShiftOp: Operator;
        AdditiveExpression: gendoc::codedom::CppObject*;
        AddOp: Operator;
        MultiplicativeExpression: gendoc::codedom::CppObject*;
        MulOp: Operator;
        PmExpression: gendoc::codedom::CppObject*;
        PmOp: Operator;
        CastExpression(var std::unique_ptr<CppObject> ce, var std::unique_ptr<CppObject> ti): gendoc::codedom::CppObject*;
        UnaryExpression(var std::unique_ptr<CppObject> ue): gendoc::codedom::CppObject*;
        UnaryOperator: Operator;
        NewExpression(var bool global, var TypeId* typeId, var bool parens): gendoc::codedom::CppObject*;
        NewTypeId(var std::unique_ptr<TypeId> ti): gendoc::codedom::TypeId*;
        NewDeclarator: std::u32string;
        DirectNewDeclarator(var std::unique_ptr<CppObject> e);
        NewPlacement: std::vector<gendoc::codedom::CppObject*>;
        NewInitializer: std::vector<gendoc::codedom::CppObject*>;
        DeleteExpression(var bool global, var bool array): gendoc::codedom::CppObject*;
        PostfixExpression(var std::unique_ptr<CppObject> pe): gendoc::codedom::CppObject*;
        TypeSpecifierOrTypeName: gendoc::codedom::CppObject*;
        PostCastExpression: gendoc::codedom::CppObject*;
        ExpressionList: std::vector<gendoc::codedom::CppObject*>;
        PrimaryExpression(var std::unique_ptr<CppObject> pe): gendoc::codedom::CppObject*;
        IdExpression: gendoc::codedom::IdExpr*;
        OperatorFunctionId;
        Operator;
    }
    grammar Literal
    {
        Literal: gendoc::codedom::Literal*;
        IntegerLiteral: gendoc::codedom::Literal*;
        DecimalLiteral;
        OctalLiteral;
        OctalDigit;
        HexadecimalLiteral;
        IntegerSuffix;
        UnsignedSuffix;
        LongLongSuffix;
        LongSuffix;
        CharacterLiteral: gendoc::codedom::Literal*;
        NarrowCharacterLiteral;
        UniversalCharacterLiteral;
        WideCharacterLiteral;
        CCharSequence: std::u32string;
        CChar;
        EscapeSequence;
        SimpleEscapeSequence;
        OctalEscapeSequence;
        HexadecimalEscapeSequence;
        FloatingLiteral: gendoc::codedom::Literal*;
        FractionalConstant;
        DigitSequence;
        ExponentPart;
        Sign;
        FloatingSuffix;
        StringLiteral: gendoc::codedom::Literal*;
        EncodingPrefix;
        SCharSequence;
        SChar;
        BooleanLiteral: gendoc::codedom::Literal*;
        PointerLiteral: gendoc::codedom::Literal*;
    }
    grammar Identifier
    {
        Identifier: std::u32string;
        QualifiedId: std::u32string;
    }
    grammar Keyword
    {
        Keyword;
    }
}
