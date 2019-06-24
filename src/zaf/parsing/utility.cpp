#include <zaf/parsing/utility.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

bool ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    float& float_value) {

    auto value = node->GetAttribute(attribute_name);
    if (! value) {
        return false;
    }

    return TryToNumeric<float>(*value, float_value);
}


bool ParseNodeAttributeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    ReflectionObject& object) {

    auto value = node->GetAttribute(attribute_name);
    if (! value) {
        return false;
    }

    object.GetType()->GetParser()->ParseFromAttribute(*value, object);
    return true;
}


bool ParseNodePropertyNodeToString(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    std::wstring& string_value) {

    auto property_node = node->GetPropertyNode(property_node_name);
    if (property_node == nullptr) {
        return false;
    }

    const auto& content_nodes = property_node->GetContentNodes();
    if (content_nodes.size() != 1) {
        return false;
    }

    const auto& content_node = content_nodes.front();
    if (content_node->GetType() != XamlNode::Type::Text) {
        return false;
    }

    string_value = content_node->GetValue();
    return true;
}


bool ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    float& float_value) {

    std::wstring string_value;
    bool is_succeeded = ParseNodePropertyNodeToString(node, property_node_name, string_value);
    if (! is_succeeded) {
        return false;
    }

    return TryToNumeric<float>(string_value, float_value);
}


bool ParseNodePropertyNodeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    ReflectionObject& object) {

    auto property_node = node->GetPropertyNode(property_node_name);
    if (property_node == nullptr) {
        return false;
    }

    object.GetType()->GetParser()->ParseFromNode(property_node, object);
    return true;
}

}