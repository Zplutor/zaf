#pragma once

#include <xmllite.h>
#include <zaf/base/com_ptr.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/stream.h>
#include <zaf/xml/xml_node_type.h>

namespace zaf {

class XMLReader : NonCopyable {
public:
    explicit XMLReader(Stream stream);

    XMLNodeType NodeType() const noexcept {
        return current_node_type_;
    }

    std::wstring_view GetName() const;
    std::wstring_view GetValue() const;

    bool Read();

    void ReadXMLDeclaration();

    void ReadUntilElementStart(std::wstring_view element_name);
    void ReadElementStart(std::wstring_view element_name);
    void ReadElementEnd();

    bool MoveToFirstAttribute();
    bool MoveToNextAttribute();
    
private:
    void ReadUntilContent();
    void ReadNode(
        XMLNodeType node_type,
        std::optional<std::wstring_view> node_name, 
        bool skip);

private:
    COMPtr<IXmlReader> inner_;
    XMLNodeType current_node_type_{ XMLNodeType::None };
};

}