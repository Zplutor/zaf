#pragma once

#include <memory>
#include <string>
#include <zaf/base/string/encoding_conversion.h>
#include <zaf/creation.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/object_type.h>

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
    const std::string& property_name,
    const std::string& property_value) {

    std::string xaml;
    xaml += "<" + type_name + ">";
    xaml += "<" + type_name + "." + property_name + ">";
    xaml += property_value;
    xaml += "</" + type_name + "." + property_name + ">";
    xaml += "</" + type_name + ">";
    return xaml;
}


template<typename T, typename ValueGetter>
bool TestBooleanProperty(const std::string& property_name, ValueGetter&& value_getter) {

    auto bool_to_string = [](bool value) -> std::string {
        return value ? "true" : "false";
    };

    std::string type_name = zaf::ToUtf8String(T::Type->GetName());

    for (auto expected_value : { true, false }) {

        std::string xaml = BuildAttributeXaml(
            type_name, 
            property_name, 
            bool_to_string(expected_value));

        auto object = zaf::CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != expected_value) {
            return false;
        }

        xaml = BuildPropertyNodeXaml(
            type_name,
            property_name,
            bool_to_string(expected_value));

        object = zaf::CreateObjectFromXaml<T>(xaml);
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

    std::string type_name = zaf::ToUtf8String(T::Type->GetName());

    for (const auto& each_pair : value_map) {

        std::string xaml = BuildAttributeXaml(type_name, property_name, each_pair.second);
        auto object = zaf::CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != each_pair.first) {
            return false;
        }

        xaml = BuildPropertyNodeXaml(type_name, property_name, each_pair.second);
        object = zaf::CreateObjectFromXaml<T>(xaml);
        if (value_getter(*object) != each_pair.first) {
            return false;
        }
    }

    return true;
}


template<typename T, typename ValueGetter>
bool TestFrameProperty(const std::string& property_name, ValueGetter&& value_getter) {

    auto create_test_frame = [](zaf::Frame& frame, std::string& string_value) {

        std::srand(unsigned int(std::time(nullptr)));
        frame = zaf::Frame(
            float(std::rand()),
            float(std::rand()),
            float(std::rand()),
            float(std::rand()));

        std::ostringstream attribute_value_stream;
        attribute_value_stream
            << frame.left << ','
            << frame.top << ','
            << frame.right << ','
            << frame.bottom;
        string_value = attribute_value_stream.str();
    };

    std::string type_name = zaf::ToUtf8String(T::Type->GetName());

    zaf::Frame test_frame;
    std::string frame_value;
    create_test_frame(test_frame, frame_value);

    auto xaml = BuildAttributeXaml(type_name, property_name, frame_value);
    auto object = zaf::CreateObjectFromXaml<T>(xaml);
    if (value_getter(*object) != test_frame) {
        return false;
    }

    create_test_frame(test_frame, frame_value);

    xaml = BuildPropertyNodeXaml(type_name, property_name, frame_value);
    object = zaf::CreateObjectFromXaml<T>(xaml);
    if (value_getter(*object) != test_frame) {
        return false;
    }

    return true;
}