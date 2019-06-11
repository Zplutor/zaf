#pragma once

#include <xmllite.h>

namespace zaf {

enum class XamlNodeType {
    None,
    Element,
    EndElement,
    Text,
};

class XamlReader {
public:
    class AttributeReader {
    public:
        AttributeReader(IXmlReader* handle);

        AttributeReader(AttributeReader&& other);
        AttributeReader& operator=(AttributeReader&& other);

        AttributeReader(const AttributeReader&) = delete;
        AttributeReader& operator=(const AttributeReader&) = delete;

        bool Read();
        std::wstring GetName() const;
        std::wstring GetValue() const;

    private:
        IXmlReader* handle_{};
    };

public:
    XamlReader(IXmlReader* handle);
    ~XamlReader();

    XamlReader(XamlReader&& other);
    XamlReader& operator=(XamlReader&& other);

    XamlReader(const XamlReader&) = delete;
    XamlReader& operator=(const XamlReader&) = delete;

    bool Next();

    XamlNodeType GetNodeType() const;
    std::wstring GetName() const;
    
    AttributeReader GetAttributeReader() const;

private:
    IXmlReader* handle_{};
};

}