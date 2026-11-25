#pragma once

#include <mutex>
#include <optional>

namespace zaf {

enum class XamlNodeType {
    Element,
    Text,
};


class XamlAttribute {
public:
    XamlAttribute(const std::wstring& name, const std::wstring& value) : 
        name_(name),
        value_(value) {

    }

    const std::wstring& Name() const {
        return name_;
    }

    const std::wstring& Value() const {
        return value_;
    }

private:
    std::wstring name_;
    std::wstring value_;
};


class XamlNode {
public:
    XamlNode() = default;
    XamlNode(const XamlNode&) = delete;
    XamlNode& operator=(const XamlNode&) = delete;

    XamlNodeType Type() const {
        return type_;
    }

    const std::wstring& Value() const {
        return value_;
    }

    bool IsPropertyNode() const;

    std::shared_ptr<XamlAttribute> FindAttribute(const std::wstring& name) const;

    const std::vector<std::shared_ptr<XamlAttribute>>& GetAttributes() const {
        return attributes_;
    }

    std::shared_ptr<XamlNode> FindPropertyNode(const std::wstring& name) const;

    const std::vector<std::shared_ptr<XamlNode>>& GetPropertyNodes() const {
        return property_nodes_;
    }

    const std::vector<std::shared_ptr<XamlNode>>& GetContentNodes() const {
        return content_nodes_;
    }

private:
    friend class XamlNodeBuilder;
    
private:
    XamlNodeType type_{ XamlNodeType::Element };
    std::wstring value_;

    std::vector<std::shared_ptr<XamlAttribute>> attributes_;
    std::vector<std::shared_ptr<XamlNode>> property_nodes_;
    std::vector<std::shared_ptr<XamlNode>> content_nodes_;

    mutable std::vector<std::shared_ptr<XamlAttribute>> sorted_attributes_;
    mutable std::once_flag sorted_attributes_once_flag_;

    mutable std::vector<std::shared_ptr<XamlNode>> sorted_property_nodes_;
    mutable std::once_flag sorted_property_nodes_once_flag_;
};

}