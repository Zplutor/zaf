#pragma once

#include <string>
#include <zaf/application.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

class ReflectionObject;

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
std::shared_ptr<T> CreateObjectFromTypeName(const std::wstring& name) {

    auto type = GetReflectionManager().GetType(name);
    if (type == nullptr) {
        return {};
    }

    auto object = type->CreateInstance();
    return std::dynamic_pointer_cast<T>(object);
}


template<typename T>
std::shared_ptr<T> CreateObjectFromNode(const std::shared_ptr<XamlNode>& node) {

    if (node->GetType() != XamlNode::Type::Element) {
        return nullptr;
    }

    auto object = CreateObjectFromTypeName<T>(node->GetValue());
    if (object == nullptr) {
        return {};
    }

    object->GetType()->GetParser()->ParseFromNode(node, *object);
    return object;
}

}
}