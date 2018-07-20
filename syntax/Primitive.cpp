#include "Primitive.hpp"
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
#include <gendoc/syntax/Element.hpp>

namespace gendoc { namespace syntax {

using namespace gendoc::parsing;
using namespace gendoc::util;
using namespace gendoc::unicode;

Primitive* Primitive::Create()
{
    return Create(new gendoc::parsing::ParsingDomain());
}

Primitive* Primitive::Create(gendoc::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Primitive* grammar(new Primitive(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Primitive::Primitive(gendoc::parsing::ParsingDomain* parsingDomain_): gendoc::parsing::Grammar(ToUtf32("Primitive"), parsingDomain_->GetNamespaceScope(ToUtf32("gendoc.syntax")), parsingDomain_)
{
    SetOwner(0);
}

gendoc::parsing::Parser* Primitive::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    gendoc::parsing::Parser* result = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class Primitive::PrimitiveRule : public gendoc::parsing::Rule
{
public:
    PrimitiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A2Action));
        gendoc::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A3Action));
        gendoc::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A4Action));
        gendoc::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A5Action));
        gendoc::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A6Action));
        gendoc::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A7Action));
        gendoc::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A8Action));
        gendoc::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A9Action));
        gendoc::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A10Action));
        gendoc::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A11Action));
        gendoc::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A12Action));
        gendoc::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A13Action));
        gendoc::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A14Action));
        gendoc::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A15Action));
        gendoc::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A16Action));
        gendoc::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A17Action));
        gendoc::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A18Action));
        gendoc::parsing::ActionParser* a19ActionParser = GetAction(ToUtf32("A19"));
        a19ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A19Action));
        gendoc::parsing::ActionParser* a20ActionParser = GetAction(ToUtf32("A20"));
        a20ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A20Action));
        gendoc::parsing::ActionParser* a21ActionParser = GetAction(ToUtf32("A21"));
        a21ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A21Action));
        gendoc::parsing::ActionParser* a22ActionParser = GetAction(ToUtf32("A22"));
        a22ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A22Action));
        gendoc::parsing::ActionParser* a23ActionParser = GetAction(ToUtf32("A23"));
        a23ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A23Action));
        gendoc::parsing::ActionParser* a24ActionParser = GetAction(ToUtf32("A24"));
        a24ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A24Action));
        gendoc::parsing::ActionParser* a25ActionParser = GetAction(ToUtf32("A25"));
        a25ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A25Action));
        gendoc::parsing::ActionParser* a26ActionParser = GetAction(ToUtf32("A26"));
        a26ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A26Action));
        gendoc::parsing::ActionParser* a27ActionParser = GetAction(ToUtf32("A27"));
        a27ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A27Action));
        gendoc::parsing::ActionParser* a28ActionParser = GetAction(ToUtf32("A28"));
        a28ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A28Action));
        gendoc::parsing::ActionParser* a29ActionParser = GetAction(ToUtf32("A29"));
        a29ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A29Action));
        gendoc::parsing::ActionParser* a30ActionParser = GetAction(ToUtf32("A30"));
        a30ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A30Action));
        gendoc::parsing::ActionParser* a31ActionParser = GetAction(ToUtf32("A31"));
        a31ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A31Action));
        gendoc::parsing::ActionParser* a32ActionParser = GetAction(ToUtf32("A32"));
        a32ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A32Action));
        gendoc::parsing::ActionParser* a33ActionParser = GetAction(ToUtf32("A33"));
        a33ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A33Action));
        gendoc::parsing::ActionParser* a34ActionParser = GetAction(ToUtf32("A34"));
        a34ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A34Action));
        gendoc::parsing::ActionParser* a35ActionParser = GetAction(ToUtf32("A35"));
        a35ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A35Action));
        gendoc::parsing::ActionParser* a36ActionParser = GetAction(ToUtf32("A36"));
        a36ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A36Action));
        gendoc::parsing::ActionParser* a37ActionParser = GetAction(ToUtf32("A37"));
        a37ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A37Action));
        gendoc::parsing::ActionParser* a38ActionParser = GetAction(ToUtf32("A38"));
        a38ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A38Action));
        gendoc::parsing::ActionParser* a39ActionParser = GetAction(ToUtf32("A39"));
        a39ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A39Action));
        gendoc::parsing::ActionParser* a40ActionParser = GetAction(ToUtf32("A40"));
        a40ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A40Action));
        gendoc::parsing::ActionParser* a41ActionParser = GetAction(ToUtf32("A41"));
        a41ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A41Action));
        gendoc::parsing::ActionParser* a42ActionParser = GetAction(ToUtf32("A42"));
        a42ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A42Action));
        gendoc::parsing::ActionParser* a43ActionParser = GetAction(ToUtf32("A43"));
        a43ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A43Action));
        gendoc::parsing::ActionParser* a44ActionParser = GetAction(ToUtf32("A44"));
        a44ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A44Action));
        gendoc::parsing::ActionParser* a45ActionParser = GetAction(ToUtf32("A45"));
        a45ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A45Action));
        gendoc::parsing::ActionParser* a46ActionParser = GetAction(ToUtf32("A46"));
        a46ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A46Action));
        gendoc::parsing::ActionParser* a47ActionParser = GetAction(ToUtf32("A47"));
        a47ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A47Action));
        gendoc::parsing::ActionParser* a48ActionParser = GetAction(ToUtf32("A48"));
        a48ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A48Action));
        gendoc::parsing::ActionParser* a49ActionParser = GetAction(ToUtf32("A49"));
        a49ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A49Action));
        gendoc::parsing::ActionParser* a50ActionParser = GetAction(ToUtf32("A50"));
        a50ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A50Action));
        gendoc::parsing::ActionParser* a51ActionParser = GetAction(ToUtf32("A51"));
        a51ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A51Action));
        gendoc::parsing::ActionParser* a52ActionParser = GetAction(ToUtf32("A52"));
        a52ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A52Action));
        gendoc::parsing::ActionParser* a53ActionParser = GetAction(ToUtf32("A53"));
        a53ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A53Action));
        gendoc::parsing::NonterminalParser* charNonterminalParser = GetNonterminal(ToUtf32("Char"));
        charNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostChar));
        gendoc::parsing::NonterminalParser* stringNonterminalParser = GetNonterminal(ToUtf32("String"));
        stringNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostString));
        gendoc::parsing::NonterminalParser* charSetNonterminalParser = GetNonterminal(ToUtf32("CharSet"));
        charSetNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCharSet));
        gendoc::parsing::NonterminalParser* keywordNonterminalParser = GetNonterminal(ToUtf32("Keyword"));
        keywordNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeyword));
        gendoc::parsing::NonterminalParser* keywordListNonterminalParser = GetNonterminal(ToUtf32("KeywordList"));
        keywordListNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeywordList));
        gendoc::parsing::NonterminalParser* rangeNonterminalParser = GetNonterminal(ToUtf32("Range"));
        rangeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostRange));
        gendoc::parsing::NonterminalParser* emptyNonterminalParser = GetNonterminal(ToUtf32("Empty"));
        emptyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEmpty));
        gendoc::parsing::NonterminalParser* spaceNonterminalParser = GetNonterminal(ToUtf32("Space"));
        spaceNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpace));
        gendoc::parsing::NonterminalParser* anyCharNonterminalParser = GetNonterminal(ToUtf32("AnyChar"));
        anyCharNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAnyChar));
        gendoc::parsing::NonterminalParser* letterNonterminalParser = GetNonterminal(ToUtf32("Letter"));
        letterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetter));
        gendoc::parsing::NonterminalParser* digitNonterminalParser = GetNonterminal(ToUtf32("Digit"));
        digitNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDigit));
        gendoc::parsing::NonterminalParser* hexDigitNonterminalParser = GetNonterminal(ToUtf32("HexDigit"));
        hexDigitNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostHexDigit));
        gendoc::parsing::NonterminalParser* upperLetterNonterminalParser = GetNonterminal(ToUtf32("UpperLetter"));
        upperLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUpperLetter));
        gendoc::parsing::NonterminalParser* lowerLetterNonterminalParser = GetNonterminal(ToUtf32("LowerLetter"));
        lowerLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLowerLetter));
        gendoc::parsing::NonterminalParser* titleLetterNonterminalParser = GetNonterminal(ToUtf32("TitleLetter"));
        titleLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostTitleLetter));
        gendoc::parsing::NonterminalParser* modifierLetterNonterminalParser = GetNonterminal(ToUtf32("ModifierLetter"));
        modifierLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierLetter));
        gendoc::parsing::NonterminalParser* otherLetterNonterminalParser = GetNonterminal(ToUtf32("OtherLetter"));
        otherLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherLetter));
        gendoc::parsing::NonterminalParser* casedLetterNonterminalParser = GetNonterminal(ToUtf32("CasedLetter"));
        casedLetterNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCasedLetter));
        gendoc::parsing::NonterminalParser* markNonterminalParser = GetNonterminal(ToUtf32("Mark"));
        markNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMark));
        gendoc::parsing::NonterminalParser* nonspacingMarkNonterminalParser = GetNonterminal(ToUtf32("NonspacingMark"));
        nonspacingMarkNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNonspacingMark));
        gendoc::parsing::NonterminalParser* enclosingMarkNonterminalParser = GetNonterminal(ToUtf32("EnclosingMark"));
        enclosingMarkNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEnclosingMark));
        gendoc::parsing::NonterminalParser* spacingMarkNonterminalParser = GetNonterminal(ToUtf32("SpacingMark"));
        spacingMarkNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpacingMark));
        gendoc::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNumber));
        gendoc::parsing::NonterminalParser* decimalNumberNonterminalParser = GetNonterminal(ToUtf32("DecimalNumber"));
        decimalNumberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDecimalNumber));
        gendoc::parsing::NonterminalParser* letterNumberNonterminalParser = GetNonterminal(ToUtf32("LetterNumber"));
        letterNumberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetterNumber));
        gendoc::parsing::NonterminalParser* otherNumberNonterminalParser = GetNonterminal(ToUtf32("OtherNumber"));
        otherNumberNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherNumber));
        gendoc::parsing::NonterminalParser* punctuationNonterminalParser = GetNonterminal(ToUtf32("Punctuation"));
        punctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPunctuation));
        gendoc::parsing::NonterminalParser* connectorPunctuationNonterminalParser = GetNonterminal(ToUtf32("ConnectorPunctuation"));
        connectorPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostConnectorPunctuation));
        gendoc::parsing::NonterminalParser* dashPunctuationNonterminalParser = GetNonterminal(ToUtf32("DashPunctuation"));
        dashPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDashPunctuation));
        gendoc::parsing::NonterminalParser* openPunctuationNonterminalParser = GetNonterminal(ToUtf32("OpenPunctuation"));
        openPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOpenPunctuation));
        gendoc::parsing::NonterminalParser* closePunctuationNonterminalParser = GetNonterminal(ToUtf32("ClosePunctuation"));
        closePunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostClosePunctuation));
        gendoc::parsing::NonterminalParser* initialPunctuationNonterminalParser = GetNonterminal(ToUtf32("InitialPunctuation"));
        initialPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostInitialPunctuation));
        gendoc::parsing::NonterminalParser* finalPunctuationNonterminalParser = GetNonterminal(ToUtf32("FinalPunctuation"));
        finalPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFinalPunctuation));
        gendoc::parsing::NonterminalParser* otherPunctuationNonterminalParser = GetNonterminal(ToUtf32("OtherPunctuation"));
        otherPunctuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherPunctuation));
        gendoc::parsing::NonterminalParser* symbolNonterminalParser = GetNonterminal(ToUtf32("Symbol"));
        symbolNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSymbol));
        gendoc::parsing::NonterminalParser* mathSymbolNonterminalParser = GetNonterminal(ToUtf32("MathSymbol"));
        mathSymbolNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMathSymbol));
        gendoc::parsing::NonterminalParser* currencySymbolNonterminalParser = GetNonterminal(ToUtf32("CurrencySymbol"));
        currencySymbolNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCurrencySymbol));
        gendoc::parsing::NonterminalParser* modifierSymbolNonterminalParser = GetNonterminal(ToUtf32("ModifierSymbol"));
        modifierSymbolNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierSymbol));
        gendoc::parsing::NonterminalParser* otherSymbolNonterminalParser = GetNonterminal(ToUtf32("OtherSymbol"));
        otherSymbolNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherSymbol));
        gendoc::parsing::NonterminalParser* separatorNonterminalParser = GetNonterminal(ToUtf32("Separator"));
        separatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSeparator));
        gendoc::parsing::NonterminalParser* spaceSeparatorNonterminalParser = GetNonterminal(ToUtf32("SpaceSeparator"));
        spaceSeparatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpaceSeparator));
        gendoc::parsing::NonterminalParser* lineSeparatorNonterminalParser = GetNonterminal(ToUtf32("LineSeparator"));
        lineSeparatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLineSeparator));
        gendoc::parsing::NonterminalParser* paragraphSeparatorNonterminalParser = GetNonterminal(ToUtf32("ParagraphSeparator"));
        paragraphSeparatorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostParagraphSeparator));
        gendoc::parsing::NonterminalParser* otherNonterminalParser = GetNonterminal(ToUtf32("Other"));
        otherNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOther));
        gendoc::parsing::NonterminalParser* controlNonterminalParser = GetNonterminal(ToUtf32("Control"));
        controlNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostControl));
        gendoc::parsing::NonterminalParser* formatNonterminalParser = GetNonterminal(ToUtf32("Format"));
        formatNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFormat));
        gendoc::parsing::NonterminalParser* surrogateNonterminalParser = GetNonterminal(ToUtf32("Surrogate"));
        surrogateNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSurrogate));
        gendoc::parsing::NonterminalParser* privateUseNonterminalParser = GetNonterminal(ToUtf32("PrivateUse"));
        privateUseNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPrivateUse));
        gendoc::parsing::NonterminalParser* unassignedNonterminalParser = GetNonterminal(ToUtf32("Unassigned"));
        unassignedNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUnassigned));
        gendoc::parsing::NonterminalParser* graphicNonterminalParser = GetNonterminal(ToUtf32("Graphic"));
        graphicNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostGraphic));
        gendoc::parsing::NonterminalParser* baseCharNonterminalParser = GetNonterminal(ToUtf32("BaseChar"));
        baseCharNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostBaseChar));
        gendoc::parsing::NonterminalParser* alphabeticNonterminalParser = GetNonterminal(ToUtf32("Alphabetic"));
        alphabeticNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAlphabetic));
        gendoc::parsing::NonterminalParser* idStartNonterminalParser = GetNonterminal(ToUtf32("IdStart"));
        idStartNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdStart));
        gendoc::parsing::NonterminalParser* idContNonterminalParser = GetNonterminal(ToUtf32("IdCont"));
        idContNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdCont));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromChar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromString;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharSet;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeyword;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordList;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRange;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmpty;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpace;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAnyChar;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetter;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDigit;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexDigit;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUpperLetter;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLowerLetter;
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTitleLetter;
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierLetter;
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherLetter;
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCasedLetter;
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMark;
    }
    void A19Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNonspacingMark;
    }
    void A20Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnclosingMark;
    }
    void A21Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpacingMark;
    }
    void A22Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void A23Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDecimalNumber;
    }
    void A24Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetterNumber;
    }
    void A25Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherNumber;
    }
    void A26Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPunctuation;
    }
    void A27Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConnectorPunctuation;
    }
    void A28Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDashPunctuation;
    }
    void A29Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOpenPunctuation;
    }
    void A30Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClosePunctuation;
    }
    void A31Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInitialPunctuation;
    }
    void A32Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFinalPunctuation;
    }
    void A33Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherPunctuation;
    }
    void A34Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSymbol;
    }
    void A35Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMathSymbol;
    }
    void A36Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCurrencySymbol;
    }
    void A37Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierSymbol;
    }
    void A38Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherSymbol;
    }
    void A39Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSeparator;
    }
    void A40Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpaceSeparator;
    }
    void A41Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLineSeparator;
    }
    void A42Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromParagraphSeparator;
    }
    void A43Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOther;
    }
    void A44Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromControl;
    }
    void A45Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFormat;
    }
    void A46Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSurrogate;
    }
    void A47Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrivateUse;
    }
    void A48Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnassigned;
    }
    void A49Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGraphic;
    }
    void A50Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBaseChar;
    }
    void A51Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAlphabetic;
    }
    void A52Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdStart;
    }
    void A53Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdCont;
    }
    void PostChar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromChar_value = std::move(stack.top());
            context->fromChar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromChar_value.get());
            stack.pop();
        }
    }
    void PostString(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromString_value = std::move(stack.top());
            context->fromString = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostCharSet(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharSet_value = std::move(stack.top());
            context->fromCharSet = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromCharSet_value.get());
            stack.pop();
        }
    }
    void PostKeyword(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromKeyword_value = std::move(stack.top());
            context->fromKeyword = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromKeyword_value.get());
            stack.pop();
        }
    }
    void PostKeywordList(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromKeywordList_value = std::move(stack.top());
            context->fromKeywordList = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromKeywordList_value.get());
            stack.pop();
        }
    }
    void PostRange(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromRange_value = std::move(stack.top());
            context->fromRange = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromRange_value.get());
            stack.pop();
        }
    }
    void PostEmpty(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEmpty_value = std::move(stack.top());
            context->fromEmpty = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromEmpty_value.get());
            stack.pop();
        }
    }
    void PostSpace(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSpace_value = std::move(stack.top());
            context->fromSpace = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSpace_value.get());
            stack.pop();
        }
    }
    void PostAnyChar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAnyChar_value = std::move(stack.top());
            context->fromAnyChar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromAnyChar_value.get());
            stack.pop();
        }
    }
    void PostLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLetter_value = std::move(stack.top());
            context->fromLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromLetter_value.get());
            stack.pop();
        }
    }
    void PostDigit(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDigit_value = std::move(stack.top());
            context->fromDigit = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromDigit_value.get());
            stack.pop();
        }
    }
    void PostHexDigit(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromHexDigit_value = std::move(stack.top());
            context->fromHexDigit = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromHexDigit_value.get());
            stack.pop();
        }
    }
    void PostUpperLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUpperLetter_value = std::move(stack.top());
            context->fromUpperLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromUpperLetter_value.get());
            stack.pop();
        }
    }
    void PostLowerLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLowerLetter_value = std::move(stack.top());
            context->fromLowerLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromLowerLetter_value.get());
            stack.pop();
        }
    }
    void PostTitleLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromTitleLetter_value = std::move(stack.top());
            context->fromTitleLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromTitleLetter_value.get());
            stack.pop();
        }
    }
    void PostModifierLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromModifierLetter_value = std::move(stack.top());
            context->fromModifierLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromModifierLetter_value.get());
            stack.pop();
        }
    }
    void PostOtherLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOtherLetter_value = std::move(stack.top());
            context->fromOtherLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOtherLetter_value.get());
            stack.pop();
        }
    }
    void PostCasedLetter(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCasedLetter_value = std::move(stack.top());
            context->fromCasedLetter = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromCasedLetter_value.get());
            stack.pop();
        }
    }
    void PostMark(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromMark_value = std::move(stack.top());
            context->fromMark = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromMark_value.get());
            stack.pop();
        }
    }
    void PostNonspacingMark(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNonspacingMark_value = std::move(stack.top());
            context->fromNonspacingMark = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromNonspacingMark_value.get());
            stack.pop();
        }
    }
    void PostEnclosingMark(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromEnclosingMark_value = std::move(stack.top());
            context->fromEnclosingMark = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromEnclosingMark_value.get());
            stack.pop();
        }
    }
    void PostSpacingMark(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSpacingMark_value = std::move(stack.top());
            context->fromSpacingMark = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSpacingMark_value.get());
            stack.pop();
        }
    }
    void PostNumber(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
    void PostDecimalNumber(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDecimalNumber_value = std::move(stack.top());
            context->fromDecimalNumber = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromDecimalNumber_value.get());
            stack.pop();
        }
    }
    void PostLetterNumber(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLetterNumber_value = std::move(stack.top());
            context->fromLetterNumber = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromLetterNumber_value.get());
            stack.pop();
        }
    }
    void PostOtherNumber(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOtherNumber_value = std::move(stack.top());
            context->fromOtherNumber = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOtherNumber_value.get());
            stack.pop();
        }
    }
    void PostPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPunctuation_value = std::move(stack.top());
            context->fromPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromPunctuation_value.get());
            stack.pop();
        }
    }
    void PostConnectorPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromConnectorPunctuation_value = std::move(stack.top());
            context->fromConnectorPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromConnectorPunctuation_value.get());
            stack.pop();
        }
    }
    void PostDashPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromDashPunctuation_value = std::move(stack.top());
            context->fromDashPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromDashPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOpenPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOpenPunctuation_value = std::move(stack.top());
            context->fromOpenPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOpenPunctuation_value.get());
            stack.pop();
        }
    }
    void PostClosePunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromClosePunctuation_value = std::move(stack.top());
            context->fromClosePunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromClosePunctuation_value.get());
            stack.pop();
        }
    }
    void PostInitialPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromInitialPunctuation_value = std::move(stack.top());
            context->fromInitialPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromInitialPunctuation_value.get());
            stack.pop();
        }
    }
    void PostFinalPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFinalPunctuation_value = std::move(stack.top());
            context->fromFinalPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromFinalPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOtherPunctuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOtherPunctuation_value = std::move(stack.top());
            context->fromOtherPunctuation = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOtherPunctuation_value.get());
            stack.pop();
        }
    }
    void PostSymbol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSymbol_value = std::move(stack.top());
            context->fromSymbol = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSymbol_value.get());
            stack.pop();
        }
    }
    void PostMathSymbol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromMathSymbol_value = std::move(stack.top());
            context->fromMathSymbol = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromMathSymbol_value.get());
            stack.pop();
        }
    }
    void PostCurrencySymbol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCurrencySymbol_value = std::move(stack.top());
            context->fromCurrencySymbol = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromCurrencySymbol_value.get());
            stack.pop();
        }
    }
    void PostModifierSymbol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromModifierSymbol_value = std::move(stack.top());
            context->fromModifierSymbol = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromModifierSymbol_value.get());
            stack.pop();
        }
    }
    void PostOtherSymbol(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOtherSymbol_value = std::move(stack.top());
            context->fromOtherSymbol = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOtherSymbol_value.get());
            stack.pop();
        }
    }
    void PostSeparator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSeparator_value = std::move(stack.top());
            context->fromSeparator = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSeparator_value.get());
            stack.pop();
        }
    }
    void PostSpaceSeparator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSpaceSeparator_value = std::move(stack.top());
            context->fromSpaceSeparator = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSpaceSeparator_value.get());
            stack.pop();
        }
    }
    void PostLineSeparator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromLineSeparator_value = std::move(stack.top());
            context->fromLineSeparator = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromLineSeparator_value.get());
            stack.pop();
        }
    }
    void PostParagraphSeparator(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromParagraphSeparator_value = std::move(stack.top());
            context->fromParagraphSeparator = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromParagraphSeparator_value.get());
            stack.pop();
        }
    }
    void PostOther(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromOther_value = std::move(stack.top());
            context->fromOther = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromOther_value.get());
            stack.pop();
        }
    }
    void PostControl(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromControl_value = std::move(stack.top());
            context->fromControl = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromControl_value.get());
            stack.pop();
        }
    }
    void PostFormat(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromFormat_value = std::move(stack.top());
            context->fromFormat = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromFormat_value.get());
            stack.pop();
        }
    }
    void PostSurrogate(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromSurrogate_value = std::move(stack.top());
            context->fromSurrogate = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromSurrogate_value.get());
            stack.pop();
        }
    }
    void PostPrivateUse(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromPrivateUse_value = std::move(stack.top());
            context->fromPrivateUse = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromPrivateUse_value.get());
            stack.pop();
        }
    }
    void PostUnassigned(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromUnassigned_value = std::move(stack.top());
            context->fromUnassigned = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromUnassigned_value.get());
            stack.pop();
        }
    }
    void PostGraphic(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromGraphic_value = std::move(stack.top());
            context->fromGraphic = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromGraphic_value.get());
            stack.pop();
        }
    }
    void PostBaseChar(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromBaseChar_value = std::move(stack.top());
            context->fromBaseChar = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromBaseChar_value.get());
            stack.pop();
        }
    }
    void PostAlphabetic(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromAlphabetic_value = std::move(stack.top());
            context->fromAlphabetic = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromAlphabetic_value.get());
            stack.pop();
        }
    }
    void PostIdStart(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdStart_value = std::move(stack.top());
            context->fromIdStart = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromIdStart_value.get());
            stack.pop();
        }
    }
    void PostIdCont(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromIdCont_value = std::move(stack.top());
            context->fromIdCont = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromIdCont_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromChar(), fromString(), fromCharSet(), fromKeyword(), fromKeywordList(), fromRange(), fromEmpty(), fromSpace(), fromAnyChar(), fromLetter(), fromDigit(), fromHexDigit(), fromUpperLetter(), fromLowerLetter(), fromTitleLetter(), fromModifierLetter(), fromOtherLetter(), fromCasedLetter(), fromMark(), fromNonspacingMark(), fromEnclosingMark(), fromSpacingMark(), fromNumber(), fromDecimalNumber(), fromLetterNumber(), fromOtherNumber(), fromPunctuation(), fromConnectorPunctuation(), fromDashPunctuation(), fromOpenPunctuation(), fromClosePunctuation(), fromInitialPunctuation(), fromFinalPunctuation(), fromOtherPunctuation(), fromSymbol(), fromMathSymbol(), fromCurrencySymbol(), fromModifierSymbol(), fromOtherSymbol(), fromSeparator(), fromSpaceSeparator(), fromLineSeparator(), fromParagraphSeparator(), fromOther(), fromControl(), fromFormat(), fromSurrogate(), fromPrivateUse(), fromUnassigned(), fromGraphic(), fromBaseChar(), fromAlphabetic(), fromIdStart(), fromIdCont() {}
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromChar;
        gendoc::parsing::Parser* fromString;
        gendoc::parsing::Parser* fromCharSet;
        gendoc::parsing::Parser* fromKeyword;
        gendoc::parsing::Parser* fromKeywordList;
        gendoc::parsing::Parser* fromRange;
        gendoc::parsing::Parser* fromEmpty;
        gendoc::parsing::Parser* fromSpace;
        gendoc::parsing::Parser* fromAnyChar;
        gendoc::parsing::Parser* fromLetter;
        gendoc::parsing::Parser* fromDigit;
        gendoc::parsing::Parser* fromHexDigit;
        gendoc::parsing::Parser* fromUpperLetter;
        gendoc::parsing::Parser* fromLowerLetter;
        gendoc::parsing::Parser* fromTitleLetter;
        gendoc::parsing::Parser* fromModifierLetter;
        gendoc::parsing::Parser* fromOtherLetter;
        gendoc::parsing::Parser* fromCasedLetter;
        gendoc::parsing::Parser* fromMark;
        gendoc::parsing::Parser* fromNonspacingMark;
        gendoc::parsing::Parser* fromEnclosingMark;
        gendoc::parsing::Parser* fromSpacingMark;
        gendoc::parsing::Parser* fromNumber;
        gendoc::parsing::Parser* fromDecimalNumber;
        gendoc::parsing::Parser* fromLetterNumber;
        gendoc::parsing::Parser* fromOtherNumber;
        gendoc::parsing::Parser* fromPunctuation;
        gendoc::parsing::Parser* fromConnectorPunctuation;
        gendoc::parsing::Parser* fromDashPunctuation;
        gendoc::parsing::Parser* fromOpenPunctuation;
        gendoc::parsing::Parser* fromClosePunctuation;
        gendoc::parsing::Parser* fromInitialPunctuation;
        gendoc::parsing::Parser* fromFinalPunctuation;
        gendoc::parsing::Parser* fromOtherPunctuation;
        gendoc::parsing::Parser* fromSymbol;
        gendoc::parsing::Parser* fromMathSymbol;
        gendoc::parsing::Parser* fromCurrencySymbol;
        gendoc::parsing::Parser* fromModifierSymbol;
        gendoc::parsing::Parser* fromOtherSymbol;
        gendoc::parsing::Parser* fromSeparator;
        gendoc::parsing::Parser* fromSpaceSeparator;
        gendoc::parsing::Parser* fromLineSeparator;
        gendoc::parsing::Parser* fromParagraphSeparator;
        gendoc::parsing::Parser* fromOther;
        gendoc::parsing::Parser* fromControl;
        gendoc::parsing::Parser* fromFormat;
        gendoc::parsing::Parser* fromSurrogate;
        gendoc::parsing::Parser* fromPrivateUse;
        gendoc::parsing::Parser* fromUnassigned;
        gendoc::parsing::Parser* fromGraphic;
        gendoc::parsing::Parser* fromBaseChar;
        gendoc::parsing::Parser* fromAlphabetic;
        gendoc::parsing::Parser* fromIdStart;
        gendoc::parsing::Parser* fromIdCont;
    };
};

class Primitive::CharRule : public gendoc::parsing::Rule
{
public:
    CharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharRule>(this, &CharRule::A0Action));
        gendoc::parsing::NonterminalParser* chrNonterminalParser = GetNonterminal(ToUtf32("chr"));
        chrNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharRule>(this, &CharRule::Postchr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharParser(context->fromchr);
    }
    void Postchr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromchr_value = std::move(stack.top());
            context->fromchr = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromchr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromchr() {}
        gendoc::parsing::Parser* value;
        char32_t fromchr;
    };
};

class Primitive::StringRule : public gendoc::parsing::Rule
{
public:
    StringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        gendoc::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<StringRule>(this, &StringRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringParser(context->fromstr);
    }
    void Poststr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromstr() {}
        gendoc::parsing::Parser* value;
        std::u32string fromstr;
    };
};

class Primitive::CharSetRule : public gendoc::parsing::Rule
{
public:
    CharSetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("inverse")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A1Action));
        gendoc::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A2Action));
        gendoc::parsing::NonterminalParser* charSetRangeNonterminalParser = GetNonterminal(ToUtf32("CharSetRange"));
        charSetRangeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRule>(this, &CharSetRule::PostCharSetRange));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharSetParser(context->s, context->inverse);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->inverse = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(context->fromCharSetRange);
    }
    void PostCharSetRange(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromCharSetRange_value = std::move(stack.top());
            context->fromCharSetRange = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromCharSetRange_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), inverse(), s(), fromCharSetRange() {}
        gendoc::parsing::Parser* value;
        bool inverse;
        std::u32string s;
        std::u32string fromCharSetRange;
    };
};

class Primitive::CharSetRangeRule : public gendoc::parsing::Rule
{
public:
    CharSetRangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A1Action));
        gendoc::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postfirst));
        gendoc::parsing::NonterminalParser* lastNonterminalParser = GetNonterminal(ToUtf32("last"));
        lastNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postlast));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
        context->value.append(1, '-');
        context->value.append(1, context->fromlast);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
    }
    void Postfirst(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postlast(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromlast_value = std::move(stack.top());
            context->fromlast = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromlast_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromfirst(), fromlast() {}
        std::u32string value;
        char32_t fromfirst;
        char32_t fromlast;
    };
};

class Primitive::CharSetCharRule : public gendoc::parsing::Rule
{
public:
    CharSetCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A1Action));
        gendoc::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CharSetCharRule>(this, &CharSetCharRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
    }
    void Postescape(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<gendoc::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromescape() {}
        char32_t value;
        char32_t fromescape;
    };
};

class Primitive::KeywordRule : public gendoc::parsing::Rule
{
public:
    KeywordRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<KeywordRule>(this, &KeywordRule::A0Action));
        gendoc::parsing::NonterminalParser* keywordBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordBody"));
        keywordBodyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<KeywordRule>(this, &KeywordRule::PostKeywordBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordBody;
    }
    void PostKeywordBody(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromKeywordBody_value = std::move(stack.top());
            context->fromKeywordBody = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromKeywordBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromKeywordBody() {}
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromKeywordBody;
    };
};

class Primitive::KeywordBodyRule : public gendoc::parsing::Rule
{
public:
    KeywordBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A1Action));
        gendoc::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Poststr));
        gendoc::parsing::NonterminalParser* continuationNonterminalParser = GetNonterminal(ToUtf32("continuation"));
        continuationNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Postcontinuation));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr, context->fromcontinuation);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr);
    }
    void Poststr(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
    void Postcontinuation(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromcontinuation_value = std::move(stack.top());
            context->fromcontinuation = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromcontinuation_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromstr(), fromcontinuation() {}
        gendoc::parsing::Parser* value;
        std::u32string fromstr;
        std::u32string fromcontinuation;
    };
};

class Primitive::KeywordListRule : public gendoc::parsing::Rule
{
public:
    KeywordListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<KeywordListRule>(this, &KeywordListRule::A0Action));
        gendoc::parsing::NonterminalParser* keywordListBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordListBody"));
        keywordListBodyNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<KeywordListRule>(this, &KeywordListRule::PostKeywordListBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordListBody;
    }
    void PostKeywordListBody(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromKeywordListBody_value = std::move(stack.top());
            context->fromKeywordListBody = *static_cast<gendoc::parsing::ValueObject<gendoc::parsing::Parser*>*>(fromKeywordListBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromKeywordListBody() {}
        gendoc::parsing::Parser* value;
        gendoc::parsing::Parser* fromKeywordListBody;
    };
};

class Primitive::KeywordListBodyRule : public gendoc::parsing::Rule
{
public:
    KeywordListBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::vector<std::u32string>"), ToUtf32("keywords")));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<KeywordListBodyRule>(this, &KeywordListBodyRule::A0Action));
        gendoc::parsing::NonterminalParser* selectorNonterminalParser = GetNonterminal(ToUtf32("selector"));
        selectorNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<KeywordListBodyRule>(this, &KeywordListBodyRule::Postselector));
        gendoc::parsing::NonterminalParser* stringArrayNonterminalParser = GetNonterminal(ToUtf32("StringArray"));
        stringArrayNonterminalParser->SetPreCall(new gendoc::parsing::MemberPreCall<KeywordListBodyRule>(this, &KeywordListBodyRule::PreStringArray));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordListParser(context->fromselector, context->keywords);
    }
    void Postselector(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromselector_value = std::move(stack.top());
            context->fromselector = *static_cast<gendoc::parsing::ValueObject<std::u32string>*>(fromselector_value.get());
            stack.pop();
        }
    }
    void PreStringArray(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<std::vector<std::u32string>*>(&context->keywords)));
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), keywords(), fromselector() {}
        gendoc::parsing::Parser* value;
        std::vector<std::u32string> keywords;
        std::u32string fromselector;
    };
};

class Primitive::RangeRule : public gendoc::parsing::Rule
{
public:
    RangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<RangeRule>(this, &RangeRule::A0Action));
        gendoc::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Posts));
        gendoc::parsing::NonterminalParser* eNonterminalParser = GetNonterminal(ToUtf32("e"));
        eNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Poste));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new RangeParser(context->froms, context->frome);
    }
    void Posts(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(froms_value.get());
            stack.pop();
        }
    }
    void Poste(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> frome_value = std::move(stack.top());
            context->frome = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(frome_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), froms(), frome() {}
        gendoc::parsing::Parser* value;
        uint32_t froms;
        uint32_t frome;
    };
};

class Primitive::CodePointRule : public gendoc::parsing::Rule
{
public:
    CodePointRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A0Action));
        gendoc::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A1Action));
        gendoc::parsing::NonterminalParser* xNonterminalParser = GetNonterminal(ToUtf32("x"));
        xNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postx));
        gendoc::parsing::NonterminalParser* dNonterminalParser = GetNonterminal(ToUtf32("d"));
        dNonterminalParser->SetPostCall(new gendoc::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postd));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromx;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromd;
    }
    void Postx(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromx_value = std::move(stack.top());
            context->fromx = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void Postd(gendoc::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<gendoc::parsing::Object> fromd_value = std::move(stack.top());
            context->fromd = *static_cast<gendoc::parsing::ValueObject<uint32_t>*>(fromd_value.get());
            stack.pop();
        }
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value(), fromx(), fromd() {}
        uint32_t value;
        uint32_t fromx;
        uint32_t fromd;
    };
};

class Primitive::EmptyRule : public gendoc::parsing::Rule
{
public:
    EmptyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EmptyRule>(this, &EmptyRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EmptyParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SpaceRule : public gendoc::parsing::Rule
{
public:
    SpaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpaceRule>(this, &SpaceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::AnyCharRule : public gendoc::parsing::Rule
{
public:
    AnyCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AnyCharRule>(this, &AnyCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AnyCharParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::LetterRule : public gendoc::parsing::Rule
{
public:
    LetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LetterRule>(this, &LetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::DigitRule : public gendoc::parsing::Rule
{
public:
    DigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DigitRule>(this, &DigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DigitParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::HexDigitRule : public gendoc::parsing::Rule
{
public:
    HexDigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<HexDigitRule>(this, &HexDigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new HexDigitParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::UpperLetterRule : public gendoc::parsing::Rule
{
public:
    UpperLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UpperLetterRule>(this, &UpperLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UpperLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::LowerLetterRule : public gendoc::parsing::Rule
{
public:
    LowerLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LowerLetterRule>(this, &LowerLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LowerLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::TitleLetterRule : public gendoc::parsing::Rule
{
public:
    TitleLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<TitleLetterRule>(this, &TitleLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TitleLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ModifierLetterRule : public gendoc::parsing::Rule
{
public:
    ModifierLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ModifierLetterRule>(this, &ModifierLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OtherLetterRule : public gendoc::parsing::Rule
{
public:
    OtherLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OtherLetterRule>(this, &OtherLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::CasedLetterRule : public gendoc::parsing::Rule
{
public:
    CasedLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CasedLetterRule>(this, &CasedLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CasedLetterParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::MarkRule : public gendoc::parsing::Rule
{
public:
    MarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MarkRule>(this, &MarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MarkParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::NonspacingMarkRule : public gendoc::parsing::Rule
{
public:
    NonspacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NonspacingMarkRule>(this, &NonspacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NonspacingMarkParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SpacingMarkRule : public gendoc::parsing::Rule
{
public:
    SpacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpacingMarkRule>(this, &SpacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpacingMarkParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::EnclosingMarkRule : public gendoc::parsing::Rule
{
public:
    EnclosingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<EnclosingMarkRule>(this, &EnclosingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnclosingMarkParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::NumberRule : public gendoc::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NumberParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::DecimalNumberRule : public gendoc::parsing::Rule
{
public:
    DecimalNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DecimalNumberRule>(this, &DecimalNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DecimalNumberParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::LetterNumberRule : public gendoc::parsing::Rule
{
public:
    LetterNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LetterNumberRule>(this, &LetterNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterNumberParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OtherNumberRule : public gendoc::parsing::Rule
{
public:
    OtherNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OtherNumberRule>(this, &OtherNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherNumberParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::PunctuationRule : public gendoc::parsing::Rule
{
public:
    PunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PunctuationRule>(this, &PunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ConnectorPunctuationRule : public gendoc::parsing::Rule
{
public:
    ConnectorPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ConnectorPunctuationRule>(this, &ConnectorPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConnectorPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::DashPunctuationRule : public gendoc::parsing::Rule
{
public:
    DashPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<DashPunctuationRule>(this, &DashPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DashPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OpenPunctuationRule : public gendoc::parsing::Rule
{
public:
    OpenPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OpenPunctuationRule>(this, &OpenPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OpenPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ClosePunctuationRule : public gendoc::parsing::Rule
{
public:
    ClosePunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ClosePunctuationRule>(this, &ClosePunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ClosePunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::InitialPunctuationRule : public gendoc::parsing::Rule
{
public:
    InitialPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<InitialPunctuationRule>(this, &InitialPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InitialPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::FinalPunctuationRule : public gendoc::parsing::Rule
{
public:
    FinalPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FinalPunctuationRule>(this, &FinalPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FinalPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OtherPunctuationRule : public gendoc::parsing::Rule
{
public:
    OtherPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OtherPunctuationRule>(this, &OtherPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherPunctuationParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SymbolRule : public gendoc::parsing::Rule
{
public:
    SymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SymbolRule>(this, &SymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SymbolParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::MathSymbolRule : public gendoc::parsing::Rule
{
public:
    MathSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<MathSymbolRule>(this, &MathSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MathSymbolParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::CurrencySymbolRule : public gendoc::parsing::Rule
{
public:
    CurrencySymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<CurrencySymbolRule>(this, &CurrencySymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CurrencySymbolParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ModifierSymbolRule : public gendoc::parsing::Rule
{
public:
    ModifierSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ModifierSymbolRule>(this, &ModifierSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierSymbolParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OtherSymbolRule : public gendoc::parsing::Rule
{
public:
    OtherSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OtherSymbolRule>(this, &OtherSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherSymbolParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SeparatorRule : public gendoc::parsing::Rule
{
public:
    SeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SeparatorRule>(this, &SeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SeparatorParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SpaceSeparatorRule : public gendoc::parsing::Rule
{
public:
    SpaceSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SpaceSeparatorRule>(this, &SpaceSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceSeparatorParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::LineSeparatorRule : public gendoc::parsing::Rule
{
public:
    LineSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<LineSeparatorRule>(this, &LineSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LineSeparatorParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ParagraphSeparatorRule : public gendoc::parsing::Rule
{
public:
    ParagraphSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ParagraphSeparatorRule>(this, &ParagraphSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParagraphSeparatorParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::OtherRule : public gendoc::parsing::Rule
{
public:
    OtherRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<OtherRule>(this, &OtherRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::ControlRule : public gendoc::parsing::Rule
{
public:
    ControlRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<ControlRule>(this, &ControlRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ControlParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::FormatRule : public gendoc::parsing::Rule
{
public:
    FormatRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<FormatRule>(this, &FormatRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FormatParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::SurrogateRule : public gendoc::parsing::Rule
{
public:
    SurrogateRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<SurrogateRule>(this, &SurrogateRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SurrogateParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::PrivateUseRule : public gendoc::parsing::Rule
{
public:
    PrivateUseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<PrivateUseRule>(this, &PrivateUseRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PrivateUseParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::UnassignedRule : public gendoc::parsing::Rule
{
public:
    UnassignedRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<UnassignedRule>(this, &UnassignedRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnassignedParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::GraphicRule : public gendoc::parsing::Rule
{
public:
    GraphicRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<GraphicRule>(this, &GraphicRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GraphicParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::BaseCharRule : public gendoc::parsing::Rule
{
public:
    BaseCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<BaseCharRule>(this, &BaseCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseCharParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::AlphabeticRule : public gendoc::parsing::Rule
{
public:
    AlphabeticRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<AlphabeticRule>(this, &AlphabeticRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlphabeticParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::IdStartRule : public gendoc::parsing::Rule
{
public:
    IdStartRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdStartRule>(this, &IdStartRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdStartParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

class Primitive::IdContRule : public gendoc::parsing::Rule
{
public:
    IdContRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        gendoc::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("gendoc::parsing::Parser*"));
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
            stack.push(std::unique_ptr<gendoc::parsing::Object>(new gendoc::parsing::ValueObject<gendoc::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        gendoc::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new gendoc::parsing::MemberParsingAction<IdContRule>(this, &IdContRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdContParser;
    }
private:
    struct Context : gendoc::parsing::Context
    {
        Context(): value() {}
        gendoc::parsing::Parser* value;
    };
};

void Primitive::GetReferencedGrammars()
{
    gendoc::parsing::ParsingDomain* pd = GetParsingDomain();
    gendoc::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("gendoc.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = gendoc::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    gendoc::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("gendoc.syntax.Element"));
    if (!grammar1)
    {
        grammar1 = gendoc::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Primitive::CreateRules()
{
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("gendoc.parsing.stdlib.escape")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("gendoc.parsing.stdlib.char")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Element.QualifiedId")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("gendoc.parsing.stdlib.string")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("int"), this, ToUtf32("gendoc.parsing.stdlib.int")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("gendoc.parsing.stdlib.hexuint")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("gendoc.parsing.stdlib.uint")));
    AddRuleLink(new gendoc::parsing::RuleLink(ToUtf32("StringArray"), this, ToUtf32("Element.StringArray")));
    AddRule(new PrimitiveRule(ToUtf32("Primitive"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::AlternativeParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::AlternativeParser(
                        new gendoc::parsing::AlternativeParser(
                            new gendoc::parsing::AlternativeParser(
                                new gendoc::parsing::AlternativeParser(
                                    new gendoc::parsing::AlternativeParser(
                                        new gendoc::parsing::AlternativeParser(
                                            new gendoc::parsing::AlternativeParser(
                                                new gendoc::parsing::AlternativeParser(
                                                    new gendoc::parsing::AlternativeParser(
                                                        new gendoc::parsing::AlternativeParser(
                                                            new gendoc::parsing::AlternativeParser(
                                                                new gendoc::parsing::AlternativeParser(
                                                                    new gendoc::parsing::AlternativeParser(
                                                                        new gendoc::parsing::AlternativeParser(
                                                                            new gendoc::parsing::AlternativeParser(
                                                                                new gendoc::parsing::AlternativeParser(
                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                            new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                                new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                                    new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                                        new gendoc::parsing::AlternativeParser(
                                                                                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                                                                                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                                                                                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                                                                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("String"), ToUtf32("String"), 0))),
                                                                                                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                                                                                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("CharSet"), ToUtf32("CharSet"), 0))),
                                                                                                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A3"),
                                                                                                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))),
                                                                                                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A4"),
                                                                                                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("KeywordList"), ToUtf32("KeywordList"), 0))),
                                                                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A5"),
                                                                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("Range"), ToUtf32("Range"), 0))),
                                                                                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A6"),
                                                                                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Empty"), ToUtf32("Empty"), 0))),
                                                                                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A7"),
                                                                                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0))),
                                                                                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A8"),
                                                                                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("AnyChar"), ToUtf32("AnyChar"), 0))),
                                                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A9"),
                                                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("Letter"), ToUtf32("Letter"), 0))),
                                                                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A10"),
                                                                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Digit"), ToUtf32("Digit"), 0))),
                                                                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A11"),
                                                                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("HexDigit"), ToUtf32("HexDigit"), 0))),
                                                                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A12"),
                                                                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("UpperLetter"), ToUtf32("UpperLetter"), 0))),
                                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A13"),
                                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("LowerLetter"), ToUtf32("LowerLetter"), 0))),
                                                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A14"),
                                                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("TitleLetter"), ToUtf32("TitleLetter"), 0))),
                                                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A15"),
                                                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("ModifierLetter"), ToUtf32("ModifierLetter"), 0))),
                                                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A16"),
                                                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("OtherLetter"), ToUtf32("OtherLetter"), 0))),
                                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A17"),
                                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("CasedLetter"), ToUtf32("CasedLetter"), 0))),
                                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A18"),
                                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Mark"), ToUtf32("Mark"), 0))),
                                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A19"),
                                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("NonspacingMark"), ToUtf32("NonspacingMark"), 0))),
                                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A20"),
                                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("EnclosingMark"), ToUtf32("EnclosingMark"), 0))),
                                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A21"),
                                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("SpacingMark"), ToUtf32("SpacingMark"), 0))),
                                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A22"),
                                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0))),
                                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A23"),
                                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("DecimalNumber"), ToUtf32("DecimalNumber"), 0))),
                                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A24"),
                                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("LetterNumber"), ToUtf32("LetterNumber"), 0))),
                                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A25"),
                                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("OtherNumber"), ToUtf32("OtherNumber"), 0))),
                                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A26"),
                                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Punctuation"), ToUtf32("Punctuation"), 0))),
                                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A27"),
                                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("ConnectorPunctuation"), ToUtf32("ConnectorPunctuation"), 0))),
                                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A28"),
                                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("DashPunctuation"), ToUtf32("DashPunctuation"), 0))),
                                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A29"),
                                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("OpenPunctuation"), ToUtf32("OpenPunctuation"), 0))),
                                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A30"),
                                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("ClosePunctuation"), ToUtf32("ClosePunctuation"), 0))),
                                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A31"),
                                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("InitialPunctuation"), ToUtf32("InitialPunctuation"), 0))),
                                                                                                new gendoc::parsing::ActionParser(ToUtf32("A32"),
                                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("FinalPunctuation"), ToUtf32("FinalPunctuation"), 0))),
                                                                                            new gendoc::parsing::ActionParser(ToUtf32("A33"),
                                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("OtherPunctuation"), ToUtf32("OtherPunctuation"), 0))),
                                                                                        new gendoc::parsing::ActionParser(ToUtf32("A34"),
                                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("Symbol"), ToUtf32("Symbol"), 0))),
                                                                                    new gendoc::parsing::ActionParser(ToUtf32("A35"),
                                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("MathSymbol"), ToUtf32("MathSymbol"), 0))),
                                                                                new gendoc::parsing::ActionParser(ToUtf32("A36"),
                                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("CurrencySymbol"), ToUtf32("CurrencySymbol"), 0))),
                                                                            new gendoc::parsing::ActionParser(ToUtf32("A37"),
                                                                                new gendoc::parsing::NonterminalParser(ToUtf32("ModifierSymbol"), ToUtf32("ModifierSymbol"), 0))),
                                                                        new gendoc::parsing::ActionParser(ToUtf32("A38"),
                                                                            new gendoc::parsing::NonterminalParser(ToUtf32("OtherSymbol"), ToUtf32("OtherSymbol"), 0))),
                                                                    new gendoc::parsing::ActionParser(ToUtf32("A39"),
                                                                        new gendoc::parsing::NonterminalParser(ToUtf32("Separator"), ToUtf32("Separator"), 0))),
                                                                new gendoc::parsing::ActionParser(ToUtf32("A40"),
                                                                    new gendoc::parsing::NonterminalParser(ToUtf32("SpaceSeparator"), ToUtf32("SpaceSeparator"), 0))),
                                                            new gendoc::parsing::ActionParser(ToUtf32("A41"),
                                                                new gendoc::parsing::NonterminalParser(ToUtf32("LineSeparator"), ToUtf32("LineSeparator"), 0))),
                                                        new gendoc::parsing::ActionParser(ToUtf32("A42"),
                                                            new gendoc::parsing::NonterminalParser(ToUtf32("ParagraphSeparator"), ToUtf32("ParagraphSeparator"), 0))),
                                                    new gendoc::parsing::ActionParser(ToUtf32("A43"),
                                                        new gendoc::parsing::NonterminalParser(ToUtf32("Other"), ToUtf32("Other"), 0))),
                                                new gendoc::parsing::ActionParser(ToUtf32("A44"),
                                                    new gendoc::parsing::NonterminalParser(ToUtf32("Control"), ToUtf32("Control"), 0))),
                                            new gendoc::parsing::ActionParser(ToUtf32("A45"),
                                                new gendoc::parsing::NonterminalParser(ToUtf32("Format"), ToUtf32("Format"), 0))),
                                        new gendoc::parsing::ActionParser(ToUtf32("A46"),
                                            new gendoc::parsing::NonterminalParser(ToUtf32("Surrogate"), ToUtf32("Surrogate"), 0))),
                                    new gendoc::parsing::ActionParser(ToUtf32("A47"),
                                        new gendoc::parsing::NonterminalParser(ToUtf32("PrivateUse"), ToUtf32("PrivateUse"), 0))),
                                new gendoc::parsing::ActionParser(ToUtf32("A48"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("Unassigned"), ToUtf32("Unassigned"), 0))),
                            new gendoc::parsing::ActionParser(ToUtf32("A49"),
                                new gendoc::parsing::NonterminalParser(ToUtf32("Graphic"), ToUtf32("Graphic"), 0))),
                        new gendoc::parsing::ActionParser(ToUtf32("A50"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("BaseChar"), ToUtf32("BaseChar"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A51"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("Alphabetic"), ToUtf32("Alphabetic"), 0))),
                new gendoc::parsing::ActionParser(ToUtf32("A52"),
                    new gendoc::parsing::NonterminalParser(ToUtf32("IdStart"), ToUtf32("IdStart"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A53"),
                new gendoc::parsing::NonterminalParser(ToUtf32("IdCont"), ToUtf32("IdCont"), 0)))));
    AddRule(new CharRule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("chr"), ToUtf32("char"), 0))));
    AddRule(new StringRule(ToUtf32("String"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0))));
    AddRule(new CharSetRule(ToUtf32("CharSet"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::CharParser('['),
                            new gendoc::parsing::OptionalParser(
                                new gendoc::parsing::GroupingParser(
                                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                                        new gendoc::parsing::CharParser('^'))))),
                        new gendoc::parsing::KleeneStarParser(
                            new gendoc::parsing::GroupingParser(
                                new gendoc::parsing::ActionParser(ToUtf32("A2"),
                                    new gendoc::parsing::NonterminalParser(ToUtf32("CharSetRange"), ToUtf32("CharSetRange"), 0))))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(']')))))));
    AddRule(new CharSetRangeRule(ToUtf32("CharSetRange"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("CharSetChar"), 0),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser('-'),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::NonterminalParser(ToUtf32("last"), ToUtf32("CharSetChar"), 0))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::EmptyParser()))))));
    AddRule(new CharSetCharRule(ToUtf32("CharSetChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::CharSetParser(ToUtf32("\\]"), true)),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))));
    AddRule(new KeywordRule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("keyword")),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('('))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("KeywordBody"), ToUtf32("KeywordBody"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new KeywordBodyRule(ToUtf32("KeywordBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::AlternativeParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::CharParser(','),
                        new gendoc::parsing::ActionParser(ToUtf32("A0"),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::NonterminalParser(ToUtf32("continuation"), ToUtf32("QualifiedId"), 0)))),
                    new gendoc::parsing::ActionParser(ToUtf32("A1"),
                        new gendoc::parsing::EmptyParser()))))));
    AddRule(new KeywordListRule(ToUtf32("KeywordList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::KeywordParser(ToUtf32("keyword_list")),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser('('))),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("KeywordListBody"), ToUtf32("KeywordListBody"), 0))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(')'))))));
    AddRule(new KeywordListBodyRule(ToUtf32("KeywordListBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::SequenceParser(
            new gendoc::parsing::SequenceParser(
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("selector"), ToUtf32("QualifiedId"), 0)),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::CharParser(','))),
            new gendoc::parsing::ActionParser(ToUtf32("A0"),
                new gendoc::parsing::ExpectationParser(
                    new gendoc::parsing::NonterminalParser(ToUtf32("StringArray"), ToUtf32("StringArray"), 1))))));
    AddRule(new RangeRule(ToUtf32("Range"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::GroupingParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::SequenceParser(
                        new gendoc::parsing::SequenceParser(
                            new gendoc::parsing::SequenceParser(
                                new gendoc::parsing::SequenceParser(
                                    new gendoc::parsing::KeywordParser(ToUtf32("range")),
                                    new gendoc::parsing::ExpectationParser(
                                        new gendoc::parsing::CharParser('('))),
                                new gendoc::parsing::ExpectationParser(
                                    new gendoc::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("CodePoint"), 0))),
                            new gendoc::parsing::ExpectationParser(
                                new gendoc::parsing::CharParser(','))),
                        new gendoc::parsing::ExpectationParser(
                            new gendoc::parsing::NonterminalParser(ToUtf32("e"), ToUtf32("CodePoint"), 0))),
                    new gendoc::parsing::ExpectationParser(
                        new gendoc::parsing::CharParser(')')))))));
    AddRule(new CodePointRule(ToUtf32("CodePoint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::AlternativeParser(
            new gendoc::parsing::TokenParser(
                new gendoc::parsing::SequenceParser(
                    new gendoc::parsing::CharParser('x'),
                    new gendoc::parsing::ActionParser(ToUtf32("A0"),
                        new gendoc::parsing::NonterminalParser(ToUtf32("x"), ToUtf32("hexuint"), 0)))),
            new gendoc::parsing::ActionParser(ToUtf32("A1"),
                new gendoc::parsing::NonterminalParser(ToUtf32("d"), ToUtf32("uint"), 0)))));
    AddRule(new EmptyRule(ToUtf32("Empty"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("empty")))));
    AddRule(new SpaceRule(ToUtf32("Space"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("space")))));
    AddRule(new AnyCharRule(ToUtf32("AnyChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("anychar")))));
    AddRule(new LetterRule(ToUtf32("Letter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("letter")))));
    AddRule(new DigitRule(ToUtf32("Digit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("digit")))));
    AddRule(new HexDigitRule(ToUtf32("HexDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("hexdigit")))));
    AddRule(new UpperLetterRule(ToUtf32("UpperLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("upper_letter")))));
    AddRule(new LowerLetterRule(ToUtf32("LowerLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("lower_letter")))));
    AddRule(new TitleLetterRule(ToUtf32("TitleLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("title_letter")))));
    AddRule(new ModifierLetterRule(ToUtf32("ModifierLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("modifier_letter")))));
    AddRule(new OtherLetterRule(ToUtf32("OtherLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("other_letter")))));
    AddRule(new CasedLetterRule(ToUtf32("CasedLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("cased_letter")))));
    AddRule(new MarkRule(ToUtf32("Mark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("mark")))));
    AddRule(new NonspacingMarkRule(ToUtf32("NonspacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("nonspacing_mark")))));
    AddRule(new SpacingMarkRule(ToUtf32("SpacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("spacing_mark")))));
    AddRule(new EnclosingMarkRule(ToUtf32("EnclosingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("enclosing_mark")))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("number")))));
    AddRule(new DecimalNumberRule(ToUtf32("DecimalNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("decimal_number")))));
    AddRule(new LetterNumberRule(ToUtf32("LetterNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("letter_number")))));
    AddRule(new OtherNumberRule(ToUtf32("OtherNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("other_number")))));
    AddRule(new PunctuationRule(ToUtf32("Punctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("punctuation")))));
    AddRule(new ConnectorPunctuationRule(ToUtf32("ConnectorPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("connector_punctuation")))));
    AddRule(new DashPunctuationRule(ToUtf32("DashPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("dash_punctuation")))));
    AddRule(new OpenPunctuationRule(ToUtf32("OpenPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("open_punctuation")))));
    AddRule(new ClosePunctuationRule(ToUtf32("ClosePunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("close_punctuation")))));
    AddRule(new InitialPunctuationRule(ToUtf32("InitialPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("initial_punctuation")))));
    AddRule(new FinalPunctuationRule(ToUtf32("FinalPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("final_punctuation")))));
    AddRule(new OtherPunctuationRule(ToUtf32("OtherPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("other_punctuation")))));
    AddRule(new SymbolRule(ToUtf32("Symbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("symbol")))));
    AddRule(new MathSymbolRule(ToUtf32("MathSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("math_symbol")))));
    AddRule(new CurrencySymbolRule(ToUtf32("CurrencySymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("currency_symbol")))));
    AddRule(new ModifierSymbolRule(ToUtf32("ModifierSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("modifier_symbol")))));
    AddRule(new OtherSymbolRule(ToUtf32("OtherSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("other_symbol")))));
    AddRule(new SeparatorRule(ToUtf32("Separator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("separator")))));
    AddRule(new SpaceSeparatorRule(ToUtf32("SpaceSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("space_separator")))));
    AddRule(new LineSeparatorRule(ToUtf32("LineSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("line_separator")))));
    AddRule(new ParagraphSeparatorRule(ToUtf32("ParagraphSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("paragraph_separator")))));
    AddRule(new OtherRule(ToUtf32("Other"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("other")))));
    AddRule(new ControlRule(ToUtf32("Control"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("control")))));
    AddRule(new FormatRule(ToUtf32("Format"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("format")))));
    AddRule(new SurrogateRule(ToUtf32("Surrogate"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("surrogate")))));
    AddRule(new PrivateUseRule(ToUtf32("PrivateUse"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("private_use")))));
    AddRule(new UnassignedRule(ToUtf32("Unassigned"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("unassigned")))));
    AddRule(new GraphicRule(ToUtf32("Graphic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("graphic")))));
    AddRule(new BaseCharRule(ToUtf32("BaseChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("basechar")))));
    AddRule(new AlphabeticRule(ToUtf32("Alphabetic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("alphabetic")))));
    AddRule(new IdStartRule(ToUtf32("IdStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("idstart")))));
    AddRule(new IdContRule(ToUtf32("IdCont"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new gendoc::parsing::ActionParser(ToUtf32("A0"),
            new gendoc::parsing::KeywordParser(ToUtf32("idcont")))));
}

} } // namespace gendoc.syntax
