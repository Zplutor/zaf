#include <zaf/parsing/xaml_node_parse_helper.h>
#include <zaf/base/string/to_numeric.h>

namespace zaf {
namespace {

std::optional<std::wstring> GetContentStringFromNode(const XamlNode& node) {

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.size() != 1) {
        return {};
    }

    const auto& content_node = content_nodes.front();
    if (content_node->GetType() != XamlNode::Type::Text) {
        return {};
    }

    return content_node->GetValue();
}

}

XamlNodeParseHelper::XamlNodeParseHelper(
    const XamlNode& node, 
    const std::shared_ptr<ReflectionType>& object_type)
    : 
    node_(node),
    object_type_(object_type) {

}


std::optional<std::wstring> XamlNodeParseHelper::GetContentString() const {
    return GetContentStringFromNode(node_);
}


std::optional<std::wstring> XamlNodeParseHelper::GetString(
    const std::wstring& property_name) const {

    auto string = GetStringFromPropertyNode(property_name);
    if (!string) {
        string = node_.GetAttribute(property_name);
    }
    return string;
}


std::optional<std::wstring> XamlNodeParseHelper::GetStringFromPropertyNode(
    const std::wstring& name) const {

    auto property_node = GetPropertyNode(name);
    if (!property_node) {
        return {};
    }

    return GetContentStringFromNode(*property_node);
}


std::optional<bool> XamlNodeParseHelper::GetBool(const std::wstring& property_name) const {

    auto string = GetString(property_name);
    if (!string) {
        return {};
    }
    
    if (*string == L"true") {
        return true;
    }
    if (*string == L"false") {
        return false;
    }
    return {};
}


std::optional<float> XamlNodeParseHelper::GetFloat(const std::wstring& property_name) const {

    auto string = GetString(property_name);
    if (!string) {
        return {};
    }

    float float_value{};
    if (TryToNumeric<float>(*string, float_value)) {
        return float_value;
    }
    return {};
}


std::shared_ptr<XamlNode> XamlNodeParseHelper::GetPropertyNode(
    const std::wstring& property_name) const {

    std::wstring name = object_type_->GetName();
    name.append(1, '.').append(property_name);
    return node_.GetPropertyNode(name);
}

}