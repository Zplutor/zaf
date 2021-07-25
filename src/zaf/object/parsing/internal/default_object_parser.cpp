#include <zaf/object/parsing/internal/default_object_parser.h>
#include <zaf/base/string/split.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/reflection/creation.h>

namespace zaf::internal {
namespace {

ObjectProperty* FindPropertyByAttribute(const Object& object, const std::wstring& property_name) {

    auto type = object.GetType();
    while (type) {

        auto property = type->FindProperty(property_name);
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

        auto property = FindPropertyByAttribute(object, each_attribute.first);
        if (!property) {
            continue;
        }

        auto value = ParsePropertyValueFromAttribute(*property, each_attribute.second);
        property->SetValue(object, value);
    }
}


ObjectProperty* FindPropertyByNode(const Object& object, const std::wstring& node_name) {

    auto splitted = Split(node_name, L'.');
    if (splitted.size() < 2) {
        return nullptr;
    }

    const auto& type_name = splitted[0];
    const auto& property_name = splitted[1];

    auto type = object.GetType();
    while (type) {

        if (type->GetName() == type_name) {

            auto property = type->FindProperty(property_name);
            if (property) {
                return property;
            }
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
            //TODO: Raise error
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

        auto property = FindPropertyByNode(object, each_node.first);
        if (!property) {
            continue;
        }

        auto value = ParsePropertyValueFromNode(*property, *each_node.second);
        property->SetValue(object, value);
    }
}

}


void DefaultObjectParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {
    //Nothing to do.
}


void DefaultObjectParser::ParseFromNode(const XamlNode& node, Object& object) {

    ParseAttributes(node, object);
    ParseNodes(node, object);
}

}