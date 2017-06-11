#pragma once

#include <memory>
#include <type_traits>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/serializable_object.h>

namespace zaf {

template<typename Type>
class Serializer {
public:
    static std::shared_ptr<DataNode> Serialize(const Type&) {
        return nullptr;
    }
};


template<>
class Serializer<std::wstring> {
public:
    static std::shared_ptr<DataNode> Serialize(const std::wstring& string) {
        return DataNode::CreateString(string);
    }
};


template<>
class Serializer<bool> {
public:
    static std::shared_ptr<DataNode> Serialize(bool boolean) {
        return DataNode::CreateBoolean(boolean);
    }
};


template<>
class Serializer<SerializableObject> {
public:
    static std::shared_ptr<DataNode> Serialize(const SerializableObject& object) {
        return object.Serialize();
    }
};


template<typename NumberType>
class NumberSerializer {
public:
    static std::shared_ptr<DataNode> Serialize(NumberType number) {
        return DataNode::CreateNumber(number);
    }
};


template<typename Type>
class GetSerializer {
public:
    typedef std::conditional_t<
        std::is_arithmetic<Type>::value, 
        NumberSerializer<Type>,
        std::conditional_t<
            std::is_base_of<SerializableObject, Type>::value, 
            Serializer<SerializableObject>, 
            Serializer<Type>
        >
    > Type;
};

}