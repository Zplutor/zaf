#pragma once

#include <zaf/dynamic/object.h>

namespace zaf {
namespace internal {

std::shared_ptr<Object> CreateReflectionObjectByName(const std::wstring& name);

}

template<typename T = Object>
std::shared_ptr<T> CreateObjectByName(const std::wstring& name) {

    auto object = internal::CreateReflectionObjectByName(name);
    return std::dynamic_pointer_cast<T>(object);
}

}