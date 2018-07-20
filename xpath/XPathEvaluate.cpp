// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/xpath/XPathEvaluate.hpp>
#include <gendoc/xpath/XPathContext.hpp>
#include <gendoc/xpath/XPath.hpp>
#include <gendoc/xpath/XPathDebug.hpp>
#include <gendoc/dom/Document.hpp>
#include <iostream>
#include <chrono>

namespace gendoc { namespace xpath {

XPath* xpathGrammar = nullptr;

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, gendoc::dom::Node* node)
{
    if (!xpathGrammar)
    {
        xpathGrammar = XPath::Create();
    }
    if (XPathDebugParsing())
    {
        xpathGrammar->SetLog(&std::cout);
    }
    std::chrono::time_point<std::chrono::steady_clock> startQuery = std::chrono::steady_clock::now();
    std::unique_ptr<XPathExpr> xpathExpr(xpathGrammar->Parse(&xpathExpression[0], &xpathExpression[0] + xpathExpression.length(), 0, ""));
    std::chrono::time_point<std::chrono::steady_clock> endQuery = std::chrono::steady_clock::now();
    if (XPathDebugQuery())
    {
        std::unique_ptr<dom::Node> queryDom = xpathExpr->ToDom();
        SetXPathQueryDom(std::move(queryDom));
        SetXPathQueryDuration(std::chrono::nanoseconds(endQuery - startQuery));
    }
    std::chrono::time_point<std::chrono::steady_clock> startEvaluate = std::chrono::steady_clock::now();
    XPathContext context(node, 1, 1);
    std::unique_ptr<XPathObject> result = xpathExpr->Evaluate(context);
    std::chrono::time_point<std::chrono::steady_clock> endEvaluate = std::chrono::steady_clock::now();
    if (XPathDebugQuery())
    {
        SetXPathExecuteDuration(endEvaluate - startEvaluate);
    }
    return result;
}

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, gendoc::dom::Document* document)
{
    return Evaluate(xpathExpression, static_cast<gendoc::dom::Node*>(document));
}

} } // namespace gendoc::xpath
