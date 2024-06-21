#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/split.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/creation.h>

namespace zaf {
namespace {

ObjectProperty* FindPropertyByAttribute(const Object& object, const std::wstring& property_name) {

    auto type = object.DynamicType();
    while (type) {

        auto property = type->GetProperty(property_name);
        if (property) {
            return property;
        }

        type = type->GetBase();
    }

    return nullptr;
}


std::shared_ptr<Object> ParsePropertyValueFromAttribute(
    const ObjectProperty& property,
    const std::wstring& attribute_value) {

    if (property.IsValueTypeDynamic()) {

        return CreateObjectByName(attribute_value);
    }
    else {

        auto value_type = property.GetValueType();
        auto value = value_type->CreateInstance();
        value_type->GetParser()->ParseFromAttribute(attribute_value, *value);
        return value;
    }
}


void ParseAttributes(const XamlNode& node, Object& object) {

    const auto& attributes = node.GetAttributes();
    for (const auto& each_attribute : attributes) {

        auto property = FindPropertyByAttribute(object, each_attribute->Name());
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

        if (current_type->GetName() == type_name) {
            return true;
        }

        current_type = current_type->GetBase();
    }
    return false;
}


ObjectProperty* FindPropertyByNode(const Object& object, const std::wstring& node_name) {

    auto splitted = Split(node_name, L'.');
    if (splitted.size() < 2) {
        return nullptr;
    }

    const auto& type_name = splitted[0];
    const auto& property_name = splitted[1];

    if (!IsTypeNameMatched(type_name, object.DynamicType())) {
        return nullptr;
    }

    auto type = object.DynamicType();
    while (type) {

        auto property = type->GetProperty(property_name);
        if (property) {
            return property;
        }

        type = type->GetBase();
    }

    return nullptr;
}


std::shared_ptr<Object> ParsePropertyValueFromNode(
    const ObjectProperty& property,
    const XamlNode& node) {

    if (property.IsValueTypeDynamic()) {

        const auto& content_nodes = node.GetContentNodes();
        if (content_nodes.size() != 1) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        return internal::CreateObjectFromNode<Object>(content_nodes.front());
    }
    else {

        auto value_type = property.GetValueType();
        auto value = value_type->CreateInstance();
        value_type->GetParser()->ParseFromNode(node, *value);
        return value;
    }
}


void ParseNodes(const XamlNode& node, Object& object) {

    const auto& property_nodes = node.GetPropertyNodes();
    for (const auto& each_node : property_nodes) {

        auto property = FindPropertyByNode(object, each_node->Value());
        if (!property) {
            continue;
        }

        auto value = ParsePropertyValueFromNode(*property, *each_node);
        property->SetValue(object, value);
    }
}

}

void ObjectParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {
    //Nothing to do.
}


void ObjectParser::ParseFromNode(const XamlNode& node, Object& object) {

    ParseAttributes(node, object);
    ParseNodes(node, object);
}

}