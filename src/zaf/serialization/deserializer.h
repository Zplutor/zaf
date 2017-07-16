#pragma once

#include <type_traits>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/serializable_object.h>

namespace zaf {

template<typename Type>
class Deserializer {
public:
    static bool Deserialize(const DataNode&, Type&) { 
        return false;
    }
};


template<>
class Deserializer<Number> {
public:
    static bool Deserialize(const DataNode& data_node, Number& number) {
        if (data_node.IsNumber()) {
            number = data_node.GetNumber();
            return true;
        }
        return false;
    }
};


template<>
class Deserializer<std::wstring> {
public:
    static bool Deserialize(const DataNode& data_node, std::wstring& string) {
        if (data_node.IsString()) {
            string = data_node.GetString();
            return true;
        }
        return false;
    }
};


template<>
class Deserializer<bool> {
public:
    static bool Deserialize(const DataNode& data_node, bool& boolean) {
        if (data_node.IsBoolean()) {
            boolean = data_node.GetBoolean();
            return true;
        }
        return false;
    }
};


template<>
class Deserializer<SerializableObject> {
public:
    static bool Deserialize(const DataNode& data_node, SerializableObject& object) {
        if (data_node.IsObject()) {
            return object.Deserialize(data_node);
        }
        return false;
    }
};


template<typename Type>
class ArithmeticDeserializer {
public:
    static bool Deserialize(const DataNode& data_node, Type& number) {
        if (data_node.IsNumber()) {
            number = data_node.GetNumber().GetValue<Type>();
            return true;
        }
        return false;
    }
};


template<typename Type>
class GetDeserializer {
public:
    typedef std::conditional_t<
        IsNumerical<Type>::Value,
        ArithmeticDeserializer<Type>,
        std::conditional_t<
            std::is_base_of<SerializableObject, Type>::value,
            Deserializer<SerializableObject>,
            Deserializer<Type>
        >
    > Type;
};

}