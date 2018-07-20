namespace gendoc.xml
{
    grammar Xml
    {
        Document(XmlProcessor* processor);
        Char;
        S;
        NameStartChar;
        NameChar;
        Name: std::u32string;
        Names;
        Nmtoken;
        Nmtokens;
        EntityValue(XmlProcessor* processor): std::u32string;
        AttValue(XmlProcessor* processor): std::u32string;
        SystemLiteral: std::u32string;
        PubidLiteral: std::u32string;
        PubidChar;
        CharData(XmlProcessor* processor);
        Comment(XmlProcessor* processor);
        PI(XmlProcessor* processor, var std::u32string data);
        PITarget: std::u32string;
        CDSect(XmlProcessor* processor);
        CDStart;
        CData: std::u32string;
        CDEnd;
        Prolog(XmlProcessor* processor);
        XMLDecl(XmlProcessor* processor);
        VersionInfo(XmlProcessor* processor);
        Eq;
        VersionNum;
        Misc(XmlProcessor* processor);
        DocTypeDecl(XmlProcessor* processor);
        DeclSep(XmlProcessor* processor);
        IntSubset(XmlProcessor* processor);
        MarkupDecl(XmlProcessor* processor);
        ExtSubset(XmlProcessor* processor);
        ExtSubsetDecl(XmlProcessor* processor);
        SDDecl(XmlProcessor* processor, var bool standalone);
        Element(XmlProcessor* processor, var std::u32string tagName);
        Attribute(XmlProcessor* processor);
        ETag(XmlProcessor* processor);
        Content(XmlProcessor* processor);
        ElementDecl(XmlProcessor* processor);
        ContentSpec;
        Children;
        CP;
        Choice;
        Seq;
        Mixed;
        AttlistDecl(XmlProcessor* processor);
        AttDef(XmlProcessor* processor);
        AttType;
        StringType;
        TokenizedType;
        EnumeratedType;
        NotationType;
        Enumeration;
        DefaultDecl(XmlProcessor* processor);
        ConditionalSect(XmlProcessor* processor);
        IncludeSect(XmlProcessor* processor);
        IgnoreSect;
        IgnoreSectContents;
        Ignore;
        CharRef: char32_t;
        Reference(XmlProcessor* processor);
        EntityRef(XmlProcessor* processor);
        PEReference(XmlProcessor* processor);
        EntityDecl(XmlProcessor* processor);
        GEDecl(XmlProcessor* processor);
        PEDecl(XmlProcessor* processor);
        EntityDef(XmlProcessor* processor);
        PEDef(XmlProcessor* processor);
        ExternalID;
        NDataDecl;
        TextDecl(XmlProcessor* processor);
        ExtParsedEnt(XmlProcessor* processor);
        EncodingDecl(XmlProcessor* processor);
        EncName: std::u32string;
        NotationDecl(XmlProcessor* processor);
        PublicID;
    }
}
