#pragma once

#include <xmllite.h>

namespace zaf {

enum class XMLNodeType {
    None = XmlNodeType_None,
    ElementStart = XmlNodeType_Element,
    Attribute = XmlNodeType_Attribute,
    Text = XmlNodeType_Text,
    CDATA = XmlNodeType_CDATA,
    ProcessingInstruction = XmlNodeType_ProcessingInstruction,
    Comment = XmlNodeType_Comment,
    DocumentType = XmlNodeType_DocumentType,
    Whitespace = XmlNodeType_Whitespace,
    ElementEnd = XmlNodeType_EndElement,
    XMLDeclaration = XmlNodeType_XmlDeclaration,
};

}