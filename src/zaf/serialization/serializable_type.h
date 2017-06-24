#pragma once

#include <functional>
#include <memory>
#include <string>
#include <zaf/creation.h>

namespace zaf {

class SerializableObject;

class SerializableType {
public:
    SerializableType() { }
    virtual ~SerializableType() { }

    /**
     Get name of the type.
     */
    virtual const wchar_t* GetName() const = 0;

    /**
     Get type info of the type.
     */
    virtual const std::type_info& GetTypeInfo() const = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<SerializableObject> CreateInstance() const = 0;

    SerializableType(const SerializableType&) = delete;
    SerializableType& operator=(const SerializableType&) = delete;
};

namespace internal {

template<typename Type>
class SerializableTypeImpl : public SerializableType {
public:
    const wchar_t* GetName() const override {
        return Type::TypeName;
    }

    const std::type_info& GetTypeInfo() const override {
        return typeid(Type);
    }

    std::shared_ptr<SerializableObject> CreateInstance() const override {
        return zaf::Create<Type>();
    }
};

}


template<typename Type>
std::shared_ptr<SerializableType> CreateSerializableType() {
    return std::make_shared<internal::SerializableTypeImpl<Type>>();
}

}