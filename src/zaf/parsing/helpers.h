#pragma once

#include <zaf/base/error.h>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/creation.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf {

void ParseObjectFromXaml(
    const std::string& xaml,
    ReflectionObject& object,
    std::error_code& error_code);

void ParseObjectFromXaml(
    const std::wstring& xaml,
    ReflectionObject& object,
    std::error_code& error_code);


template<typename T = ReflectionObject>
std::shared_ptr<T> CreateObjectFromXaml(const std::string& xaml, std::error_code& error_code) {

    auto object = Create<T>();
    ParseObjectFromXaml(xaml, *object, error_code);
    if (!error_code) {
        return object;
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
