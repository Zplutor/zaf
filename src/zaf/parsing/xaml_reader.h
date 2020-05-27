#pragma once

#include <xmllite.h>
#include <zaf/parsing/xaml_node.h>

namespace zaf {

class XamlReader {
public:
    static std::shared_ptr<XamlReader> CreateFromString(const std::wstring& xaml);
    static std::shared_ptr<XamlReader> CreateFromString(const std::string& xaml);

public:
    XamlReader(IXmlReader* handle);
    ~XamlReader();

    XamlReader(const XamlReader&) = delete;
    XamlReader& operator=(const XamlReader&) = delete;

    std::shared_ptr<XamlNode> Read();

private:
    HRESULT ReadRootNode(std::shared_ptr<XamlNode>& root_node);
    HRESULT ReadElementNode(std::shared_ptr<XamlNode>& node);
    HRESULT ReadAttributes(XamlNode& node);
    HRESULT ReadChildren(XamlNode& node);
    HRESULT ReadTextNode(std::shared_ptr<XamlNode>& node);
    HRESULT AdvanceToNextNode(XmlNodeType& next_node_type);

private:
    IXmlReader* handle_{};
};

}