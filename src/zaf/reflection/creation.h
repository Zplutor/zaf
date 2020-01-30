#pragma once

#include <zaf/reflection/reflection_object.h>

namespace zaf {
namespace internal {

std::shared_ptr<ReflectionObject> CreateReflectionObjectByName(const std::wstring& name);

}

template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectByName(const std::wstring& name) {

    auto object = internal::CreateReflectionObjectByName(name);
    return std::dynamic_pointer_cast<T>(object);
}

}