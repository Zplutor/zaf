#pragma once

#include <string>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/creation.h>

namespace zaf {

namespace internal {

bool ParseAttributeToDoubleFloats(
    const std::wstring& attribute, 
    float& float0,
    float& float1);

bool ParseAttributeToQuaterFloats(
    const std::wstring& attribute, 
    float& float0,
    float& float1,
    float& float2,
    float& float3);


template<typename T>
std::shared_ptr<T> CreateObjectFromNode(const std::shared_ptr<XamlNode>& node) {

    if (node->Type() != XamlNodeType::Element) {
        return {};
    }

    auto object = CreateObjectByName<T>(node->Value());
    if (!object) {
        return {};
    }

    object->DynamicType()->Parser()->ParseFromNode(*node, *object);
    return object;
}

}
}