#pragma once

#include <xmllite.h>
#include <functional>
#include <zaf/base/com_ptr.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/xml/xml_input_options.h>
#include <zaf/xml/xml_node_type.h>

namespace zaf {

class XMLAttributeReader;

class XMLReader : NonCopyable {
public:
    struct ElementInfo {
        bool is_empty_element{};
    };

public:
    explicit XMLReader(Stream stream);
    XMLReader(Stream stream, const XMLInputOptions& options);

    bool Read();

    XMLNodeType GetNodeType() const;
    std::wstring_view GetName() const;
    std::wstring_view GetValue() const;
    bool IsEmptyElement() const noexcept;

    bool MoveToFirstAttribute();
    bool MoveToNextAttribute();
    void MoveToElement();

    void ReadXMLDeclaration();

    ElementInfo ReadElementStart(std::wstring_view element_name);
    void ReadElementEnd();

    void ReadNotEmptyElementStart(std::wstring_view element_name);
    bool TryReadNotEmptyElementStart(std::wstring_view element_name);

    ElementInfo ReadUntilElement(std::wstring_view element_name);
    void ReadUntilNotEmptyElement(std::wstring_view element_name);

    std::wstring GetAttributeValue(const std::wstring& attribute_name) const;

    void ReadElementAttributes(
        std::wstring_view element_name,
        const std::function<void(const XMLAttributeReader&)>& visitor);

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