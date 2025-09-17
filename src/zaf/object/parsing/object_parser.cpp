#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/split.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/reflection.h>

namespace zaf {
namespace {

std::shared_ptr<Object> ParsePropertyValueFromAttribute(
    const ObjectProperty& property,
    const std::wstring& attribute_value) {

    if (property.IsValueDynamic()) {

        return Reflection::CreateObject(attribute_value);
    }
    else {

        auto value_type = property.ValueType();
        auto value = value_type->CreateInstance();
        value_type->Parser()->ParseFromAttribute(attribute_value, *value);
        return value;
    }
}


void ParseAttributes(const XamlNode& node, Object& object) {

    const auto& attributes = node.GetAttributes();
    for (const auto& each_attribute : attributes) {

        auto type = object.DynamicType();
        auto property = type->GetProperty(each_attribute->Name());
        if (!property) {
            continue;
        }

        auto value = ParsePropertyValueFromAttribute(*property, each_attribute->Value());
        property->SetValue(object, value);
    }
}


bool IsTypeNameMatched(const std::wstring& type_name, ObjectType* type) {

    auto current_type = type;
    while (current_type) {

        if (current_type->Name() == type_name) {
            return true;
        }

        current_type = current_type->BaseType();
    }
    return false;
}


std::shared_ptr<Object> ParsePropertyValueFromNode(
    const ObjectProperty& property,
    const XamlNode& node) {

    if (property.IsValueDynamic()) {

        const auto& content_nodes = node.GetContentNodes();
        if (content_nodes.size() != 1) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        return internal::CreateObjectFromNode<Object>(content_nodes.front());
    }
    else {

        auto value_type = property.ValueType();
        auto value = value_type->CreateInstance();
        value_type->Parser()->ParseFromNode(node, *value);
        return value;
    }
}

}

void ObjectParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {
    //Nothing to do.
}


void ObjectParser::ParseFromNode(const XamlNode& node, Object& object) {

    ParseAttributes(node, object);
    ParsePropertyNodes(node, object);
}


void ObjectParser::ParsePropertyNodes(const XamlNode& node, Object& object) {

    const auto& property_nodes = node.GetPropertyNodes();
    for (const auto& each_node : property_nodes) {
        ParseSinglePropertyNode(*each_node, object);
    }
}


void ObjectParser::ParseSinglePropertyNode(const XamlNode& node, Object& object) {

    auto splitted = Split(node.Value(), L'.');
    if (splitted.size() < 2) {
        return;
    }

    const auto& type_name = splitted[0];
    const auto& property_name = splitted[1];

    if (!IsTypeNameMatched(type_name, object.DynamicType())) {
        return;
    }

    ParsePropertyNode(node, property_name, object);
}


void ObjectParser::ParsePropertyNode(
    const XamlNode& node,
    const std::wstring& property_name, 
    Object& object) {

    auto property = object.DynamicType()->GetProperty(property_name);
    if (!property) {
        return;
    }

    auto value = ParsePropertyValueFromNode(*property, node);
    property->SetValue(object, value);
}

}