// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef GENDOC_DOM_NODE_INCLUDED
#define GENDOC_DOM_NODE_INCLUDED
#include <gendoc/util/CodeFormatter.hpp>
#include <string>
#include <memory>
#include <vector>

namespace gendoc { namespace dom {

using namespace gendoc::util;

enum class NodeType
{
    attributeNode, documentFragmentNode, documentNode, documentTypeNode, elementNode, entityNode, entityReferenceNode, notationNode, processingInstructionNode, textNode, cdataSectionNode, commentNode
};

class Node;
class ParentNode;
class Document;
class NodeList;
class Element;
class Text;
class EntityReference;
class CDataSection;
class Comment;
class ProcessingInstruction;

class Visitor
{
public:
    virtual void BeginVisit(Document* document) {}
    virtual void EndVisit(Document* document) {}
    virtual void BeginVisit(Element* element) {}
    virtual void EndVisit(Element* element) {}
    virtual void Visit(Text* text) {}
    virtual void Visit(CDataSection* cdataSection) {}
    virtual void Visit(Comment* comment) {}
    virtual void Visit(EntityReference* entityReference) {}
    virtual void Visit(ProcessingInstruction* processingInstruction) {}
};

class NodeOp
{
public:
    virtual ~NodeOp();
    virtual void Apply(Node* node);
};

enum class Axis
{
    child, descendant, parent, ancestor, followingSibling, precedingSibling, following, preceding, attribute, ns, self, descendantOrSelf, ancestorOrSelf
};

std::u32string AxisName(Axis axis);

class Node
{
public:
    Node(NodeType nodeType_, const std::u32string& name_);
    virtual ~Node();
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;
    virtual std::unique_ptr<Node> CloneNode(bool deep) = 0;
    NodeType GetNodeType() const { return nodeType; }
    const std::u32string& Name() const { return name; }
    const std::u32string& NamespaceUri() const { return namespaceUri; }
    std::u32string Prefix() const;
    void SetPrefix(const std::u32string& prefix);
    std::u32string LocalName() const;
    virtual bool HasChildNodes() const { return false; }
    virtual bool HasAttributes() const { return false; }
    virtual void Write(CodeFormatter& formatter) = 0;
    virtual bool ValueContainsNewLine() const { return false; }
    virtual void Accept(Visitor& visitor) {}
    void Walk(NodeOp& nodeOp, Axis axis);
    virtual void WalkChildren(NodeOp& nodeOp);
    virtual void WalkDescendant(NodeOp& nodeOp);
    virtual void WalkDescendantOrSelf(NodeOp& nodeOp);
    void WalkParent(NodeOp& nodeOp);
    virtual void WalkFollowing(NodeOp& nodeOp);
    virtual void WalkPreceding(NodeOp& nodeOp);
    virtual void WalkPrecedingOrSelf(NodeOp& nodeOp);
    void WalkAncestor(NodeOp& nodeOp);
    void WalkAncestorOrSelf(NodeOp& nodeOp);
    void WalkFollowingSibling(NodeOp& nodeOp);
    void WalkPrecedingSibling(NodeOp& nodeOp);
    virtual void WalkAttribute(NodeOp& nodeOp);
    Node* PreviousSibling() const { return previousSibling; }
    Node* NextSibling() const { return nextSibling; }
    ParentNode* Parent() const { return parent; }
    Document* OwnerDocument() const { return ownerDocument; }
    void InternalLinkBefore(Node* newNode);
    void InternalLinkAfter(Node* newNode);
    void InternalUnlink();
    void InternalSetParent(ParentNode* parent_) { parent = parent_; }
    void InternalSetOwnerDocument(Document* ownerDocument_) { ownerDocument = ownerDocument_; }
    void InternalSetNamespaceUri(const std::u32string& namespaceUri_);
private:
    NodeType nodeType;
    std::u32string name;
    std::u32string namespaceUri;
    ParentNode* parent;
    Node* previousSibling;
    Node* nextSibling;
    Document* ownerDocument;
};

class ParentNode : public Node
{
public:
    ParentNode(NodeType nodeType_, const std::u32string& name_);
    ~ParentNode() override;
    ParentNode(const Node&) = delete;
    ParentNode& operator=(const Node&) = delete;
    ParentNode(ParentNode&&) = delete;
    ParentNode& operator=(ParentNode&&) = delete;
    bool HasChildNodes() const override { return firstChild != nullptr; }
    NodeList ChildNodes() const;
    void Write(CodeFormatter& formatter) override;
    void CloneChildrenTo(ParentNode* clone) const;
    Node* FirstChild() const { return firstChild; }
    Node* LastChild() const { return lastChild; }
    virtual Node* InsertBefore(std::unique_ptr<Node>&& newChild, Node* refChild);
    virtual std::unique_ptr<Node> ReplaceChild(std::unique_ptr<Node>&& newChild, Node* oldChild);
    virtual std::unique_ptr<Node> RemoveChild(Node* oldChild);
    virtual Node* AppendChild(std::unique_ptr<Node>&& newChild);
    void Accept(Visitor& visitor) override;
    void WalkChildren(NodeOp& nodeOp) override;
    void WalkDescendant(NodeOp& nodeOp) override;
    void WalkDescendantOrSelf(NodeOp& nodeOp) override;
    void WalkPreceding(NodeOp& nodeOp) override;
    void WalkPrecedingOrSelf(NodeOp& nodeOp) override;
private:
    Node* firstChild;
    Node* lastChild;
};

class NodeList
{
public:
    Node* Item(int index) const { return nodes[index]; }
    Node* operator[](int index) const { return nodes[index]; }
    int Length() const { return nodes.size(); }
    void InternalAddNode(Node* node);
private:
    std::vector<Node*> nodes;
};

} } // namespace gendoc::dom

#endif // GENDOC_DOM_NODE_INCLUDED
