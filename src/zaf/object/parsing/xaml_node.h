#pragma once

#include <optional>

namespace zaf {

class XamlNode {
public:
    enum class Type {
        Element,
        Text,
    };

public:
    XamlNode(Type type) : type_(type) { }

    XamlNode(const XamlNode&) = delete;
    XamlNode& operator=(const XamlNode&) = delete;

    Type GetType() const {
        return type_;
    }

    const std::wstring& GetValue() const {
        return value_;
    }

    void SetValue(const std::wstring& value) {
        value_ = value;
    }

    bool IsPropertyNode() const;

    void AddAttribute(const std::wstring& name, const std::wstring& value);
    std::optional<std::wstring> GetAttribute(const std::wstring& name) const;

    const std::map<std::wstring, std::wstring>& GetAttributes() const {
        return attributes_;
    }

    void AddChildNode(const std::shared_ptr<XamlNode>& node);

    std::shared_ptr<XamlNode> GetPropertyNode(const std::wstring& name) const;

    const std::map<std::wstring, std::shared_ptr<XamlNode>>& GetPropertyNodes() const {
        return property_nodes_;
    }

    const std::vector<std::shared_ptr<XamlNode>>& GetContentNodes() const {
        return content_nodes_;
    }

private:
    Type type_{ Type::Element };
    std::wstring value_;
    std::map<std::wstring, std::wstring> attributes_;
    std::map<std::wstring, std::shared_ptr<XamlNode>> property_nodes_;
    std::vector<std::shared_ptr<XamlNode>> content_nodes_;
};

}