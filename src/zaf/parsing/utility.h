#pragma once

#include <memory>
#include <zaf/creation.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

std::optional<bool> ParseNodeAttributeToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name);


std::optional<float> ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name);

bool ParseNodeAttributeToFloat(
    const std::shared_ptr<XamlNode>& node, 
    const std::wstring& attribute_name,
    float& float_value);


template<typename T>
std::optional<T> ParseNodeAttributeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name) {

    auto attribute = node->GetAttribute(attribute_name);
    if (! attribute) {
        return {};
    }

    T object;
    T::Type->GetParser()->ParseFromAttribute(*attribute, object);
    return std::move(object);
}

template<typename T>
bool ParseNodeAttributeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    T& object) {

    auto parsed_object = ParseNodeAttributeToObject<T>(node, attribute_name);
    if (parsed_object) {
        object = std::move(*parsed_object);
        return true;
    }
    return false;
}


template<typename T>
std::shared_ptr<T> ParseNodeAttributeToPointer(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name) {

    auto attribute = node->GetAttribute(attribute_name);
    if (! attribute) {
        return nullptr;
    }

    auto pointer = Create<T>();
    T::Type->GetParser()->ParseFromAttribute(*attribute, *pointer);
    return pointer;
}


std::optional<std::wstring> ParseNodePropertyNodeToString(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name);


std::optional<bool> ParseNodePropertyNodeToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name);


std::optional<float> ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name);

bool ParseNodePropertyNodeToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    float& float_value);


template<typename T>
std::optional<T> ParseNodePropertyNodeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name) {

    auto property_node = node->GetPropertyNode(property_node_name);
    if (property_node == nullptr) {
        return {};
    }

    T object;
    T::Type->GetParser()->ParseFromNode(property_node, object);
    return std::move(object);
}

template<typename T>
bool ParseNodePropertyNodeToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name,
    T& object) {

    auto parsed_object = ParseNodePropertyNodeToObject<T>(node, property_node_name);
    if (parsed_object) {
        object = std::move(*parsed_object);
        return true;
    }
    return false;
}


template<typename T>
std::shared_ptr<T> ParseNodePropertyNodeToPointer(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& property_node_name) {

    auto property_node = node->GetPropertyNode(property_node_name);
    if (property_node == nullptr) {
        return nullptr;
    }

    auto pointer = Create<T>();
    T::Type->GetParser()->ParseFromNode(property_node, *pointer);
    return pointer;
}


std::optional<std::wstring> ParseNodeChildToString(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name);


std::optional<bool> ParseNodeChildToBool(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name);


std::optional<float> ParseNodeChildToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name);

bool ParseNodeChildToFloat(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name,
    float& float_value);


template<typename T>
std::optional<T> ParseNodeChildToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name) {

    auto object = ParseNodePropertyNodeToObject<T>(node, property_node_name);
    if (object) {
        return object;
    }

    return ParseNodeAttributeToObject<T>(node, attribute_name);
}

template<typename T>
bool ParseNodeChildToObject(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name,
    T& object) {

    auto parsed_object = ParseNodeChildToObject<T>(node, attribute_name, property_node_name);
    if (parsed_object) {
        object = std::move(*parsed_object);
        return true;
    }
    return false;
}


template<typename T>
std::shared_ptr<T> ParseNodeChildToPointer(
    const std::shared_ptr<XamlNode>& node,
    const std::wstring& attribute_name,
    const std::wstring& property_node_name) {

    auto pointer = ParseNodePropertyNodeToPointer(node, propert_node_name);
    if (pointer != nullptr) {
        return pointer;
    }

    return ParseNodeAttributeToPointer(node, attribute_name);
}

}