#pragma once

#include <zaf/internal/object/shared_ptr_utility.h>
#include <zaf/object/boxing/boxing_traits.h>
#include <zaf/object/custom_property_value_traits.h>

namespace zaf {

template<typename T, typename = void>
struct PropertyValueTraits { };

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<HasBoxingTraitsV<T>>> {

    using ValueType = std::decay_t<T>;
    using BoxedType = typename BoxingTraits<T>::BoxedType;
    
    static std::shared_ptr<Object> ToBoxedObject(ValueType value) {
        return BoxingTraits<T>::Box(std::move(value));
    }

    static const ValueType& FromBoxedObject(const std::shared_ptr<Object>& object) {
        return *BoxingTraits<T>::Unbox(*object);
    }
};

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<IsBoxedInstanceTypeV<T>>> {

    using ValueType = std::decay_t<T>;
    using BoxedType = typename ValueType::element_type;

    static std::shared_ptr<Object> ToBoxedObject(T value) {
        return std::move(value);
    }

    static T FromBoxedObject(const std::shared_ptr<Object>& object) {
        return As<typename ValueType::element_type>(object);
    }
};

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<HasCustomPropertyValueTraitsV<std::decay_t<T>>>> {
private:
    using CustomTraits = CustomPropertyValueTraits<std::decay_t<T>>;

public:
    using ValueType = std::decay_t<T>;
    using BoxedType = typename CustomTraits::BoxedType;

    static std::shared_ptr<Object> ToBoxedObject(T value) {
        return CustomTraits::ToBoxedObject(std::move(value));
    }

    static T FromBoxedObject(const std::shared_ptr<Object>& object) {
        auto custom_boxed_object = As<BoxedType>(object);
        if (custom_boxed_object) {
            return CustomTraits::FromBoxedObject(custom_boxed_object);
        }
        throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
    }
};

}