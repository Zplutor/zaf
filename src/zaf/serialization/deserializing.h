#pragma once 

#include <zaf/creation.h>
#include <zaf/serialization/deserializer.h>

namespace zaf {
namespace internal {

std::shared_ptr<SerializableObject> DeserializeObject(const DataNode& data_node);
std::shared_ptr<DataNode> ParseDataNode(bool is_xml, const void* string, std::size_t length);

}

template<typename Type>
Type Deserialize(const DataNode& data_node) {

    Type value{ };
    TryDeserialize(data_node, value);
    return value;
}


template<typename Type>
bool TryDeserialize(const DataNode& data_node, Type& value) {
    return GetDeserializer<Type>::Type::Deserialize(data_node, value);
}


template<typename Type>
std::shared_ptr<Type> DeserializeObject(const DataNode& data_node) {

    auto object = std::dynamic_pointer_cast<Type>(internal::DeserializeObject(data_node));
    if (object != nullptr) {
        return object;
    }

    object = zaf::Create<Type>();
    if (object->Deserialize(data_node)) {
        return object;
    }

    return nullptr;
}


template<typename Type>
std::shared_ptr<Type> DeserializeObjectFromXml(const void* xml, std::size_t length) {
    
    auto data_node = internal::ParseDataNode(true, xml, length);
    if (data_node != nullptr) {
        return DeserializeObject<Type>(*data_node);
    }
    return nullptr;
}


template<typename Type>
std::shared_ptr<Type> DeserializeObjectFromXml(const std::string& xml) {
    return DeserializeObjectFromXml<Type>(xml.data(), xml.length());
}


template<typename Type>
std::shared_ptr<Type> DeserializeObjectFromJson(const void* json, std::size_t length) {

    auto data_node = internal::ParseDataNode(false, json, length);
    if (data_node != nullptr) {
        return DeserializeObject<Type>(*data_node);
    }
    return nullptr;
}


template<typename Type>
std::shared_ptr<Type> DeserializeObjectFromJson(const std::string& json) {
    return DeserializeObjectFromJson<Type>(json.data(), json.length());
}


}