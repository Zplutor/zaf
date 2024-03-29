#pragma once

#include <xmllite.h>
#include <zaf/object/parsing/xaml_node.h>

namespace zaf {

class Stream;
class XamlNodeBuilder;

class XamlReader {
public:
    static std::shared_ptr<XamlReader> FromString(const std::wstring& xaml);
    static std::shared_ptr<XamlReader> FromString(const std::string& xaml);
    static std::shared_ptr<XamlReader> FromStream(const Stream& stream);

public:
    XamlReader(IXmlReader* handle);
    ~XamlReader();

    XamlReader(const XamlReader&) = delete;
    XamlReader& operator=(const XamlReader&) = delete;

    std::shared_ptr<XamlNode> Read();

private:
    HRESULT ReadRootNode(std::shared_ptr<XamlNode>& root_node);
    HRESULT ReadElementNode(std::shared_ptr<XamlNode>& node);
    HRESULT ReadAttributes(XamlNodeBuilder& node_builder);
    HRESULT ReadChildren(XamlNodeBuilder& node_builder);
    HRESULT ReadTextNode(std::shared_ptr<XamlNode>& node);
    HRESULT AdvanceToNextNode(XmlNodeType& next_node_type);

private:
    IXmlReader* handle_{};
};

}