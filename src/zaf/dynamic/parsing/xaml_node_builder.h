#pragma once

#include <zaf/dynamic/parsing/xaml_node.h>

namespace zaf {

class XamlNodeBuilder {
public:
    XamlNodeBuilder() : node_(std::make_shared<XamlNode>()) {

    }

    ~XamlNodeBuilder() = default;

    XamlNodeBuilder(const XamlNodeBuilder&) = delete;
    XamlNodeBuilder& operator=(const XamlNodeBuilder&) = delete;

    void SetType(XamlNodeType type) {
        node_->type_ = type;
    }

    void SetValue(const std::wstring& value) {
        node_->value_ = value;
    }

    void AddAttribute(const std::wstring& name, const std::wstring& value) {
        node_->attributes_.push_back(std::make_shared<XamlAttribute>(name, value));
    }

    void AddChildNode(const std::shared_ptr<XamlNode>& node) {
        if (node->IsPropertyNode()) {
            node_->property_nodes_.push_back(node);
        }
        else {
            node_->content_nodes_.push_back(node);
        }
    }

    std::shared_ptr<XamlNode> Build() {
        return std::move(node_);
    }

private:
    std::shared_ptr<XamlNode> node_;
};

}