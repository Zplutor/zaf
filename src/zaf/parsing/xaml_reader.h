#pragma once

#include <xmllite.h>
#include <zaf/base/error.h>
#include <zaf/parsing/xaml_node.h>

namespace zaf {

class XamlReader {
public:
    static std::shared_ptr<XamlReader> CreateFromString(
        const std::wstring& xaml,
        std::error_code& error);

    static std::shared_ptr<XamlReader> CreateFromString(const std::wstring& xaml) {
        std::error_code error;
        auto result = CreateFromString(xaml, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

    static std::shared_ptr<XamlReader> CreateFromString(
        const std::string& xaml, 
        std::error_code& error);

    static std::shared_ptr<XamlReader> CreateFromString(const std::string& xaml) {
        std::error_code error;
        auto result = CreateFromString(xaml, error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

public:
    XamlReader(IXmlReader* handle);
    ~XamlReader();

    XamlReader(const XamlReader&) = delete;
    XamlReader& operator=(const XamlReader&) = delete;

    std::shared_ptr<XamlNode> Read(std::error_code& error);
    std::shared_ptr<XamlNode> Read() {
        std::error_code error;
        auto result = Read(error);
        ZAF_CHECK_ERROR(error);
        return result;
    }

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