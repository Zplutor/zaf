#pragma once

#include <string>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/creation.h>

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

    if (node->GetType() != XamlNode::Type::Element) {
        return nullptr;
    }

    auto object = CreateObjectByName<T>(node->GetValue());
    if (object == nullptr) {
        return {};
    }

    object->GetType()->GetParser()->ParseFromNode(*node, *object);
    return object;
}

}
}