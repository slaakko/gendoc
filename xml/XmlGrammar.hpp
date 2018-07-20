#ifndef XmlGrammar_hpp_29675
#define XmlGrammar_hpp_29675

#include <gendoc/parsing/Grammar.hpp>
#include <gendoc/parsing/Keyword.hpp>
#include <gendoc/xml/XmlProcessor.hpp>

namespace gendoc { namespace xml {

class Xml : public gendoc::parsing::Grammar
{
public:
    static Xml* Create();
    static Xml* Create(gendoc::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, XmlProcessor* processor);
private:
    Xml(gendoc::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class DocumentRule;
    class NameRule;
    class EntityValueRule;
    class AttValueRule;
    class SystemLiteralRule;
    class PubidLiteralRule;
    class CharDataRule;
    class CommentRule;
    class PIRule;
    class PITargetRule;
    class CDSectRule;
    class CDataRule;
    class PrologRule;
    class XMLDeclRule;
    class VersionInfoRule;
    class MiscRule;
    class DocTypeDeclRule;
    class DeclSepRule;
    class IntSubsetRule;
    class MarkupDeclRule;
    class ExtSubsetRule;
    class ExtSubsetDeclRule;
    class SDDeclRule;
    class ElementRule;
    class AttributeRule;
    class ETagRule;
    class ContentRule;
    class ElementDeclRule;
    class AttlistDeclRule;
    class AttDefRule;
    class DefaultDeclRule;
    class ConditionalSectRule;
    class IncludeSectRule;
    class CharRefRule;
    class ReferenceRule;
    class EntityRefRule;
    class PEReferenceRule;
    class EntityDeclRule;
    class GEDeclRule;
    class PEDeclRule;
    class EntityDefRule;
    class PEDefRule;
    class TextDeclRule;
    class ExtParsedEntRule;
    class EncodingDeclRule;
    class EncNameRule;
    class NotationDeclRule;
};

} } // namespace gendoc.xml

#endif // XmlGrammar_hpp_29675
