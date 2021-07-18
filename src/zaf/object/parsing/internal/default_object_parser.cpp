#include <zaf/object/parsing/internal/default_object_parser.h>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>
#include <zaf/object/object_type.h>
#include <zaf/object/parsing/xaml_node.h>

namespace zaf::internal {
namespace {

bool ParsePropertyWithAttribute(
    const XamlNode& node,
    const ObjectProperty& property,
    Object& object) {

    auto attribute_value = node.GetAttribute(property.GetName());
    if (!attribute_value) {
        return false;
    }

    auto value_type = property.GetValueType();
    auto value = value_type->CreateInstance();
    value_type->GetParser()->ParseFromAttribute(*attribute_value, *value);

    property.SetValue(object, value);
    return true;
}


bool ParsePropertyWithNode(const XamlNode& node, const ObjectProperty& property, Object& object) {

    std::wstring node_name = object.GetType()->GetName();
    node_name += '.';
    node_name += property.GetName();

    auto property_node = node.GetPropertyNode(node_name);
    if (!property_node) {
        return false;
    }

    auto value_type = property.GetValueType();
    auto value = value_type->CreateInstance();
    value_type->GetParser()->ParseFromNode(*property_node, *value);

    property.SetValue(object, value);
    return true;
}


void ParsePropertiesInType(const XamlNode& node, const ObjectType& type, Object& object) {

    const auto& properties = type.GetProperties();
    for (const auto& each_property : properties) {

        if (!each_property->CanSet()) {
            continue;
        }

        if (!ParsePropertyWithNode(node, *each_property, object)) {
            ParsePropertyWithAttribute(node, *each_property, object);
        }
    }
}

}


void DefaultObjectParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {
    //Nothing to do.
}


void DefaultObjectParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto type = object.GetType();
    while (type) {

        ParsePropertiesInType(node, *type, object);

        type = type->GetBase();
    }
}

}