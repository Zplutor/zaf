#pragma once

#include <memory>
#include <optional>

namespace zaf {

class ReflectionObject;
class XamlNode;

bool ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node, 
    const std::wstring& attribute_name,
    float& float_value);

bool ParseNodeAttributeToObject(
    const std::shared_ptr<XamlNode>& node, 
    const std::wstring& attribute_name,
    ReflectionObject& object);

bool ParseNodePropertyNodeToString(
    const std::shared_ptr<XamlNode>& node, 
    const std::wstring& property_node_name,
    std::wstring& string_value);

bool ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    float& float_value);

bool ParseNodePropertyNodetoObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    ReflectionObject& object);

}