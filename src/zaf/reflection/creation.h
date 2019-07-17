#pragma once

#include <zaf/application.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {

template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectByName(const std::wstring& name) {

    auto type = GetReflectionManager().GetType(name);
    if (type == nullptr) {
        return nullptr;
    }

    auto object = type->CreateInstance();
    return std::dynamic_pointer_cast<T>(object);
}

}