#include <zaf/object/parsing/xaml_node.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>

namespace zaf {

bool XamlNode::IsPropertyNode() const {

    if (GetType() != XamlNode::Type::Element) {
        return false;
    }

    const auto& value = GetValue();
    return Contain(value, L'.');
}


void XamlNode::AddAttribute(const std::wstring& name, const std::wstring& value) {
    attributes_[name] = value;
}


std::optional<std::wstring> XamlNode::GetAttribute(const std::wstring& name) const {

    auto value = Find(attributes_, name);
    if (value != nullptr) {
        return *value;
    }
    return {};
}


void XamlNode::AddChildNode(const std::shared_ptr<XamlNode>& node) {

    auto node_type = node->GetType();
    if (node_type == XamlNode::Type::Element) {

        if (node->IsPropertyNode()) {
            property_nodes_[node->GetValue()] = node;
        }
        else {
            content_nodes_.push_back(node);
        }
    }
    else if (node_type == XamlNode::Type::Text) {
        content_nodes_.push_back(node);
    }
}


std::shared_ptr<XamlNode> XamlNode::GetPropertyNode(const std::wstring& name) const {

    auto node = Find(property_nodes_, name);
    if (node != nullptr) {
        return *node;
    }
    return nullptr;
}

}