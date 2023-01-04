#pragma once

#include <zaf/base/string/encoding_conversion.h>
#include <zaf/object/object.h>

namespace zaf {
namespace internal {

std::shared_ptr<Object> CreateReflectionObjectFromXaml(const std::string& xaml);

}


template<typename T = Object>
std::shared_ptr<T> CreateObjectFromXaml(const std::string& xaml) {
    auto object = internal::CreateReflectionObjectFromXaml(xaml);
    return std::dynamic_pointer_cast<T>(object);
}


template<typename T = Object>
std::shared_ptr<T> CreateObjectFromXaml(const std::wstring& xaml) {
    return CreateObjectFromXaml<T>(ToUTF8String(xaml));
}

}
