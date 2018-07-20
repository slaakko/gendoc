// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_XPATH_XPATH_OBJECT
#define GENDOC_XPATH_XPATH_OBJECT
#include <gendoc/dom/Node.hpp>

namespace gendoc { namespace xpath {

enum class XPathObjectType
{
    nodeSet, boolean, number, string
};

class XPathObject
{
public:
    XPathObject(XPathObjectType type_);
    XPathObjectType Type() const { return type; }
    virtual ~XPathObject();
    virtual std::unique_ptr<dom::Node> ToDom() const = 0;
private:
    XPathObjectType type;
};

class XPathNodeSet : public XPathObject
{
public:
    XPathNodeSet();
    gendoc::dom::Node* operator[](int index) const { return nodes[index]; }
    int Length() const { return nodes.Length(); }
    void Add(gendoc::dom::Node* node);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    gendoc::dom::NodeList nodes;
};

class XPathBoolean : public XPathObject
{
public:
    XPathBoolean(bool value_);
    bool Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    bool value;
};

class XPathNumber : public XPathObject
{
public:
    XPathNumber(double value_);
    double Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    double value;
};

class XPathString : public XPathObject
{
public:    
    XPathString(const std::u32string& value_);
    const std::u32string& Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string value;
};

} } // namespace gendoc::xpath

#endif // GENDOC_XPATH_XPATH_OBJECT
