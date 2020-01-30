#pragma once

#include <memory>
#include <string>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/creation.h>
#include <zaf/parsing/helpers.h>
#include <zaf/parsing/parser.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/reflection_type.h>

inline std::string BuildAttributeXaml(
    const std::string& type_name, 
    const std::string& attribute_name,
    const std::string& attribute_value) {

    std::string xaml;
    xaml += "<" + type_name + " ";
    xaml += attribute_name;
    xaml += "=\"" + attribute_value + "\"/>";
    return xaml;
}


inline std::string BuildPropertyNodeXaml(
    const std::string& type_name,
    const std::string& attribute_name,
    const std::string& attribute_value) {

    std::string xaml;
    xaml += "<" + type_name + ">";
    xaml += "<" + type_name + "." + attribute_name + ">";
    xaml += attribute_value;
    xaml += "</" + type_name + "." + attribute_name + ">";
    xaml += "</" + type_name + ">";
    return xaml;
}


template<typename T, typename ValueGetter>
bool TestBooleanProperty(const std::string& property_name, ValueGetter&& value_getter) {

    auto bool_to_string = [](bool value) -> std::string {
        return value ? "true" : "false";
    };

    std::string type_name = ToUtf8String(T::Type->GetName());

    for (auto expected_value : { true, false }) {

        std::string xaml = BuildAttributeXaml(
            type_name, 
            property_name, 
            bool_to_string(expected_value));

        auto object = CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != expected_value) {
            return false;
        }

        xaml = BuildPropertyNodeXaml(
            type_name,
            property_name,
            bool_to_string(expected_value));

        object = CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != expected_value) {
            return false;
        }
    }

    return true;
}


template<typename T, typename E, typename ValueGetter>
bool TestEnumProperty(
    const std::string& property_name, 
    const std::map<E, std::string>& value_map, 
    ValueGetter&& value_getter) {

    std::string type_name = ToUtf8String(T::Type->GetName());

    for (const auto& each_pair : value_map) {

        std::string xaml = BuildAttributeXaml(type_name, property_name, each_pair.second);
        auto object = CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != each_pair.first) {
            return false;
        }

        xaml = BuildPropertyNodeXaml(type_name, property_name, each_pair.second);
        object = CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != each_pair.first) {
            return false;
        }
    }

    return true;
}