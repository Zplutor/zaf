#pragma once 

#include <zaf/creation.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/json/json_reader.h>

template<typename Type>
std::shared_ptr<Type> DeserializeObject(const std::string& json) {

    zaf::JsonReader reader;

    std::error_code error_code;
    auto data_node = reader.Read(json, error_code);
    if (data_node == nullptr) {
        return nullptr;
    }

    auto object = zaf::Create<Type>();
    bool is_succeeded = object->Deserialize(*data_node);
    if (! is_succeeded) {
        return nullptr;
    }

    return object;
}