#pragma once

#include <zaf/base/error.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf {
namespace internal {

std::shared_ptr<ReflectionObject> CreateReflectionObjectFromXaml(
    const std::string& xaml,
    std::error_code& error_code);

}


template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectFromXaml(const std::string& xaml, std::error_code& error_code) {

    auto object = internal::CreateReflectionObjectFromXaml(xaml, error_code);
    if (!error_code) {
        return std::dynamic_pointer_cast<T>(object);
    }
    return {};
}

template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectFromXaml(const std::string& xaml) {

    std::error_code error_code;
    auto object = CreateObjectFromXaml<T>(xaml, error_code);
    ZAF_CHECK_ERROR(error_code);
    return object;
}


template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectFromXaml(const std::wstring& xaml, std::error_code& error_code) {
    return CreateObjectFromXaml<T>(ToUtf8String(xaml), error_code);
}

template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectFromXaml(const std::wstring& xaml) {
    return CreateObjectFromXaml<T>(ToUtf8String(xaml));
}

}
