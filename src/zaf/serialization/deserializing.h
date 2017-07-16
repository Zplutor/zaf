#pragma once 

#include <zaf/creation.h>
#include <zaf/serialization/deserializer.h>

namespace zaf {
namespace internal {

std::shared_ptr<SerializableObject> DeserializeObject(const DataNode& data_node);

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


}