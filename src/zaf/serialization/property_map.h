#pragma once

#include <functional>
#include <map>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/serializer.h>

namespace zaf {

/**
 Reprensents a container that stores name-value properties.

 The types of values stored in PropertyMap must be copyable.
*/
class PropertyMap {
public:
    class Value {
    public:
        Value() { }
        virtual ~Value() { }

        virtual std::shared_ptr<DataNode> Serialize() const = 0;

        Value(const Value&) = delete;
        Value& operator=(const Value&) = delete;
    };

    template<typename Type>
    class TypedValue : public Value {
    public:
        TypedValue(const Type& value) : value_(value) { }
        TypedValue(Type&& value) : value_(std::forward<Type>(value)) { }

        const Type* GetValuePointer() const {
            return &value_;
        }

        Type* GetValuePointer() {
            return &value_;
        }

        std::shared_ptr<DataNode> Serialize() const override {
            typedef typename GetSerializer<Type>::Type SerializerType;
            return SerializerType::Serialize(value_);
        }

    private:
        Type value_;
    };

public:
    /**
     Try to get the value of specified property.

     @param PropertyType
         The type of the value expected to get.

     @param property_name
         The name of the property.

     @return
         A pointer to the value of the property. Return nullptr if the property is not found,
         or the actual type of value doesn't match PropertyType.
     */
    template<typename PropertyType>
    const PropertyType* TryGetProperty(const std::wstring& property_name) const {

        auto iterator = properties_.find(property_name);
        if (iterator == properties_.end()) {
            return nullptr;
        }

        return Cast<PropertyType>(iterator->second);;
    }

    template<typename PropertyType>
    PropertyType* TryGetProperty(const std::wstring& property_name) {
        auto property = static_cast<const PropertyMap*>(this)->TryGetProperty<PropertyType>(property_name);
        return const_cast<PropertyType*>(property);
    }

    /**
     Get the value of specified property, or set a new value to the property if the specified value
     cannot be got.

     @param PropertyType
         The type of the value.

     @param property_name
         The name of the property.

     @param property_creator
         A function object used to create a value. The returned value would be set to
         the property if it is not found or the type of value doesn't match PropertyType.
         This parameter can not be nullptr.

     @return
         A reference to the value of the property.
     */
    template<typename PropertyType>
    PropertyType& GetProperty(const std::wstring& property_name, const std::function<PropertyType()>& property_creator) {

        auto property = TryGetProperty<PropertyType>(property_name);
        if (property != nullptr) {
            return *property;
        }

        return SetProperty(property_name, property_creator());
    }

    /**
     Set a property with specified name and value.

     @param property_name
         The name of the property.

     @param property_value
         The value of the property.
     */
    template<typename PropertyType>
    PropertyType& SetProperty(const std::wstring& property_name, const PropertyType& property_value) {
        auto value = std::make_shared<GetTypedValueType<PropertyType>::Type>(property_value);
        properties_[property_name] = value;
        return *value->GetValuePointer();
    }

    void RemoveProperty(const std::wstring& property_name) {
        properties_.erase(property_name);
    }

    void EnumerateProperties(const std::function<void(const std::wstring&, const Value&)>& enumerator) const {
        for (const auto& each_pair : properties_) {
            enumerator(each_pair.first, *each_pair.second);
        }
    }

private:
    template<typename PropertyType>
    class GetTypedValueType {
    public:
        typedef TypedValue<std::decay_t<std::remove_cv_t<PropertyType>>> Type;
    };

    template<typename PropertyType>
    static const PropertyType* Cast(const std::shared_ptr<Value>& value) {

        auto typed_value = dynamic_cast<GetTypedValueType<PropertyType>::Type*>(value.get());
        if (typed_value != nullptr) {
            return typed_value->GetValuePointer();
        }
        return nullptr;
    }

private:
    std::map<std::wstring, std::shared_ptr<Value>> properties_;
};

}