#include <zaf/parsing/utility.h>
#include <zaf/base/string/to_numeric.h>

namespace zaf {
namespace {

std::optional<bool> StringToBool(const std::wstring& string) {
    if (string == L"true") {
        return true;
    }
    if (string == L"false") {
        return false;
    }
    return {};
}

}

std::optional<bool> ParseNodeAttributeToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name) {

    auto value = node->GetAttribute(attribute_name);
    if (! value) {
        return {};
    }

    return StringToBool(*value);
}


std::optional<float> ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name) {

    auto value = node->GetAttribute(attribute_name);
    if (! value) {
        return {};
    }

    float float_value{};
    if (TryToNumeric<float>(*value, float_value)) {
        return float_value;
    }

    return {};
}

bool ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    float& float_value) {

    auto value = ParseNodeAttributeToFloat(node, attribute_name);
    if (value) {
        float_value = *value;
        return true;
    }
    return false;
}


std::optional<std::wstring> ParseNodePropertyNodeToString(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name) {

    auto property_node = node->GetPropertyNode(property_node_name);
    if (property_node == nullptr) {
        return {};
    }

    const auto& content_nodes = property_node->GetContentNodes();
    if (content_nodes.size() != 1) {
        return {};
    }

    const auto& content_node = content_nodes.front();
    if (content_node->GetType() != XamlNode::Type::Text) {
        return {};
    }

    return content_node->GetValue();
}


std::optional<bool> ParseNodePropertyNodeToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name) {

    auto string = ParseNodePropertyNodeToString(node, property_node_name);
    if (string) {
        return StringToBool(*string);
    }
    return {};
}


std::optional<float> ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name) {

    auto string = ParseNodePropertyNodeToString(node, property_node_name);
    if (! string) {
        return {};
    }

    float float_value{};
    if (TryToNumeric<float>(*string, float_value)) {
        return float_value;
    }
    return {};
}


bool ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    float& float_value) {

    auto parsed_float = ParseNodePropertyNodeToFloat(node, property_node_name);
    if (parsed_float) {
        float_value = *parsed_float;
        return true;
    }
    return false;
}


std::optional<std::wstring> ParseNodeChildToString(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name) {

    auto parsed_string = ParseNodePropertyNodeToString(node, property_node_name);
    if (parsed_string) {
        return *parsed_string;
    }

    return node->GetAttribute(attribute_name);
}


std::optional<bool> ParseNodeChildToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name) {

    auto parsed_bool= ParseNodePropertyNodeToBool(node, property_node_name);
    if (parsed_bool) {
        return parsed_bool;
    }

    return ParseNodeAttributeToBool(node, attribute_name);
}


std::optional<float> ParseNodeChildToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name) {

    auto parsed_float = ParseNodePropertyNodeToFloat(node, property_node_name);
    if (parsed_float) {
        return parsed_float;
    }

    return ParseNodeAttributeToFloat(node, attribute_name);
}

bool ParseNodeChildToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name,
    float& float_value) {

    auto parsed_float = ParseNodeChildToFloat(node, attribute_name, property_node_name);
    if (parsed_float) {
        float_value = *parsed_float;
        return true;
    }
    return false;
}


}