#pragma once

#include <memory>
#include <string>
#include <zaf/creation.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/reflection_type.h>

template<typename T>
std::shared_ptr<T> CreateObjectFromXaml(const std::string& xaml) {

    auto node = zaf::XamlReader::CreateFromString(xaml)->Read();

    auto object = zaf::Create<T>();
    T::Type->GetParser()->ParseFromNode(*node, *object);

    return object;
}