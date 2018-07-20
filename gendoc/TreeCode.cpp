// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <gendoc/gendoc/TreeCode.hpp>

namespace gendoc { namespace html {

void GenerateTreeCode(CodeFormatter& formatter)
{
    formatter.WriteLine("function drawTreeNodes(nodes) {");
    formatter.WriteLine("   var maxTextWidth = 0;");
    formatter.WriteLine("   var maxTextHeight = 0;");
    formatter.WriteLine("   var maxLevel = 0;");
    formatter.WriteLine("   var xspace = 8;");
    formatter.WriteLine("   var yspace = 8;");
    formatter.WriteLine("   var rectXSpace = 40;");
    formatter.WriteLine("   var rectYSpace = 20;");
    formatter.WriteLine("   var svg = document.getElementById(\"treeDiagram\");");
    formatter.WriteLine("   var n = nodes.length;");
    formatter.WriteLine("   for (var i = 0; i < n; ++i) {");
    formatter.WriteLine("       var node = nodes[i];");
    formatter.WriteLine("       var nodeLevel = node.level;");
    formatter.WriteLine("       if (nodeLevel > maxLevel) {");
    formatter.WriteLine("           maxLevel = nodeLevel;");
    formatter.WriteLine("       }");
    formatter.WriteLine("       var rectElement = document.createElementNS('http://www.w3.org/2000/svg', 'rect');");
    formatter.WriteLine("       rectElement.setAttribute(\"id\", \"rect\" + i.toString());");
    formatter.WriteLine("       svg.appendChild(rectElement);");
    formatter.WriteLine("       var textElement = document.createElementNS('http://www.w3.org/2000/svg', 'text');");
    formatter.WriteLine("       if (node.id && node.id != \"\") {");
    formatter.WriteLine("           textElement.setAttribute(\"id\", node.id);"); 
    formatter.WriteLine("       } else {");
    formatter.WriteLine("           textElement.setAttribute(\"id\", \"text\" + i.toString());");
    formatter.WriteLine("       }");
    formatter.WriteLine("       textElement.innerHTML = node.name;");
    formatter.WriteLine("       var hasLink = node.link && node.link != \"\";");
    formatter.WriteLine("       if (hasLink) {");
    formatter.WriteLine("           if (node.link.indexOf(\".html\") == -1) {");
    formatter.WriteLine("               textElement.targetId = node.link;");
    formatter.WriteLine("               textElement.onclick = function() {");
    formatter.WriteLine("                   var targetTextElement = document.getElementById(this.targetId);");
    formatter.WriteLine("                   if (targetTextElement) {");
    formatter.WriteLine("                       var h = targetTextElement.getBBox().y - this.getBBox().y;");
    formatter.WriteLine("                       window.scrollBy(0, h);");
    formatter.WriteLine("                   }");
    formatter.WriteLine("               };");
    formatter.WriteLine("               svg.appendChild(textElement);");
    formatter.WriteLine("           } else {");
    formatter.WriteLine("               var linkElement = document.createElementNS('http://www.w3.org/2000/svg', 'a');");
    formatter.WriteLine("               linkElement.setAttribute(\"href\", node.link);");
    formatter.WriteLine("               linkElement.appendChild(textElement);");
    formatter.WriteLine("               svg.appendChild(linkElement);");
    formatter.WriteLine("           }");
    formatter.WriteLine("       } else {");
    formatter.WriteLine("           svg.appendChild(textElement);");
    formatter.WriteLine("       }");
    formatter.WriteLine("       var bb = textElement.getBBox();");
    formatter.WriteLine("       var textWidth = bb.width;");
    formatter.WriteLine("       var textHeight = bb.height;");
    formatter.WriteLine("       node.textWidth = textWidth;");
    formatter.WriteLine("       node.textHeight = textHeight;");
    formatter.WriteLine("       if (textWidth > maxTextWidth) {");
    formatter.WriteLine("           maxTextWidth = textWidth;");
    formatter.WriteLine("       }");
    formatter.WriteLine("       if (textHeight > maxTextHeight) {");
    formatter.WriteLine("           maxTextHeight = textHeight;");
    formatter.WriteLine("       }");
    formatter.WriteLine("   }");
    formatter.WriteLine("   var rectWidth = maxTextWidth + 2 * xspace;");
    formatter.WriteLine("   var rectHeight = maxTextHeight + 2 * yspace;");
    formatter.WriteLine("   var totalWidth = rectWidth + maxLevel * rectXSpace;");
    formatter.WriteLine("   var totalHeight = n * (rectHeight + rectYSpace);");
    formatter.WriteLine("   svg.setAttribute(\"width\", totalWidth.toString());");
    formatter.WriteLine("   svg.setAttribute(\"height\", totalHeight.toString());");
    formatter.WriteLine("   var levelNode = [];");
    formatter.WriteLine("   for (var i = 0; i < n; ++i) {");
    formatter.WriteLine("       var node = nodes[i];");
    formatter.WriteLine("       var level = node.level;");
    formatter.WriteLine("       if (level == levelNode.length) {");
    formatter.WriteLine("           levelNode.push(node);");
    formatter.WriteLine("       }");
    formatter.WriteLine("       else {");
    formatter.WriteLine("           levelNode[level] = node;");
    formatter.WriteLine("       }");
    formatter.WriteLine("       var rectX = level * rectXSpace;");
    formatter.WriteLine("       var rectY = i * (rectHeight + rectYSpace);");
    formatter.WriteLine("       var levelX = rectX + rectXSpace / 2;");
    formatter.WriteLine("       var levelY = rectY + rectHeight;");
    formatter.WriteLine("       node.x = levelX;");
    formatter.WriteLine("       node.y = levelY;");
    formatter.WriteLine("       var textWidth = node.textWidth;");
    formatter.WriteLine("       var textHeight = node.textHeight;");
    formatter.WriteLine("       var textX = rectX + (rectWidth - textWidth) / 2;");
    formatter.WriteLine("       var textY = (rectY + rectHeight - yspace / 2) - (rectHeight - textHeight) / 2;");
    formatter.WriteLine("       var rectElement = document.getElementById(\"rect\" + i.toString());");
    formatter.WriteLine("       rectElement.setAttribute(\"x\", rectX.toString());");
    formatter.WriteLine("       rectElement.setAttribute(\"y\", rectY.toString());");
    formatter.WriteLine("       rectElement.setAttribute(\"width\", rectWidth.toString());");
    formatter.WriteLine("       rectElement.setAttribute(\"height\", rectHeight.toString());");
    formatter.WriteLine("       rectElement.setAttribute(\"fill\", \"white\");");
    formatter.WriteLine("       rectElement.setAttribute(\"stroke\", \"black\");");
    formatter.WriteLine("       var textId = \"text\" + i.toString();");
    formatter.WriteLine("       if (node.id && node.id != \"\") {");
    formatter.WriteLine("           textId = node.id;");
    formatter.WriteLine("       }");
    formatter.WriteLine("       var textElement = document.getElementById(textId);");
    formatter.WriteLine("       textElement.setAttribute(\"x\", textX.toString());");
    formatter.WriteLine("       textElement.setAttribute(\"y\", textY.toString());");
    formatter.WriteLine("       if (level > 0) {");
    formatter.WriteLine("           var x1 = rectX;");
    formatter.WriteLine("           var y1 = rectY + rectHeight / 2;");
    formatter.WriteLine("           var x2 = levelNode[level - 1].x;");
    formatter.WriteLine("           var y2 = levelNode[level - 1].y;");
    formatter.WriteLine("           var horLineElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');");
    formatter.WriteLine("           horLineElement.setAttribute(\"x1\", x1.toString());");
    formatter.WriteLine("           horLineElement.setAttribute(\"y1\", y1.toString());");
    formatter.WriteLine("           horLineElement.setAttribute(\"x2\", x2.toString());");
    formatter.WriteLine("           horLineElement.setAttribute(\"y2\", y1.toString());");
    formatter.WriteLine("           horLineElement.setAttribute(\"stroke\", \"black\");");
    formatter.WriteLine("           svg.appendChild(horLineElement);");
    formatter.WriteLine("           var verLineElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');");
    formatter.WriteLine("           verLineElement.setAttribute(\"x1\", x2.toString());");
    formatter.WriteLine("           verLineElement.setAttribute(\"y1\", y1.toString());");
    formatter.WriteLine("           verLineElement.setAttribute(\"x2\", x2.toString());");
    formatter.WriteLine("           verLineElement.setAttribute(\"y2\", y2.toString());");
    formatter.WriteLine("           verLineElement.setAttribute(\"stroke\", \"black\");");
    formatter.WriteLine("           svg.appendChild(verLineElement);");
    formatter.WriteLine("       }");
    formatter.WriteLine("   }");
    formatter.WriteLine("}");
}

} } // namespace gendoc::html
