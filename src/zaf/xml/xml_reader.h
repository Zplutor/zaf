#pragma once

#include <xmllite.h>
#include <functional>
#include <zaf/base/code_page.h>
#include <zaf/base/com_ptr.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/xml/xml_node_type.h>

namespace zaf {

class XMLAttributeReader;

class XMLReader : NonCopyable {
public:
    explicit XMLReader(Stream stream);
    XMLReader(Stream stream, CodePage code_page);

    XMLNodeType GetNodeType() const;

    std::wstring_view GetName() const;
    std::wstring_view GetValue() const;

    bool IsEmptyElement() const noexcept;

    bool Read();

    void ReadXMLDeclaration();

    void ReadUntilElement(std::wstring_view element_name);
    void ReadElementStart(std::wstring_view element_name);
    void ReadElementEnd();
    bool TryReadElementStart(std::wstring_view element_name);

    void ReadElementAttributes(
        std::wstring_view element_name,
        const std::function<void(const XMLAttributeReader&)>& visitor);

    std::wstring_view ReadCDATA();

    bool MoveToFirstAttribute();
    bool MoveToNextAttribute();
    void MoveToElement();
    
private:
    void ReadUntilContent();

    void ReadNode(
        XMLNodeType node_type,
        std::optional<std::wstring_view> node_name, 
        bool skip);

    bool TryReadNode(
        XMLNodeType node_type,
        std::optional<std::wstring_view> node_name,
        bool skip);

private:
    COMPtr<IXmlReader> inner_;
};


class XMLAttributeReader : NonCopyable {
public:
    explicit XMLAttributeReader(XMLReader& reader) : reader_(reader) {

    }

    std::wstring_view GetName() const {
        return reader_.GetName();
    }

    std::wstring_view GetString() const {
        return reader_.GetValue();
    }

    template<typename T>
    T GetNumber() const {
        return ToNumeric<T>(GetString().data());
    }

private:
    XMLReader& reader_;
};

}