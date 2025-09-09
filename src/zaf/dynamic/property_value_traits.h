#pragma once

/**
@file
    Defines type utilities related to the property value boxing and unboxing mechanism.
*/

#include <zaf/base/type_traits/optional.h>
#include <zaf/dynamic/boxing/boxing.h>
#include <zaf/dynamic/custom_property_value_traits.h>

namespace zaf {

/**
A template for getting the corresponding boxed type and performing boxing/unboxing operations for
the specified property value type.

@tparam T
    The type for which the boxed type and boxing/unboxing operations are to be defined. It should
    be the return type of the property getter or the argument type of the property setter.

@details
    This template is used by the property mechanism to convert values between the declared type and
    the boxed type of the property value. Declared type is the type defined by users for the
    property getter and setter, while boxed type is the type used by the property mechanism
    internally to operate on the property value. For example, given the following property
    getter:

    @code
    int Value() const;
    @endcode

    The declared type is `int`, and the boxed type is `zaf::Int32`.

    The primary template doesn't define any members and will emit a compile error if initiated,
    indicating that the specified type doesn't support being used for properties.

    If the specified `T` supports being used for properties, the following members will be defined
    in this template:

    - A type alias `ValueType` that defines the decayed type of `T`. It is equal to
      `std::decay_t<T>`.

    - A type alias `BoxedType` that defines the boxed type of `T`.

    - A static method `ToBoxedObject` that converts the property value to a boxed instance. The
      signature of the method is:

      @code
      std::shared_ptr<zaf::Object> ToBoxedObject(T&& value);
      @endcode

    - A static method `FromBoxedObject` that converts a boxed instance to the property value. The
      signature of the method is:

      @code
      ValueType FromBoxedObject(const std::shared_ptr<zaf::Object>& object);
      @endcode

    The following types support being used for properties:
    - Types that support boxing and unboxing, indicated by `zaf::HasBoxingTraitsV<T>` being true.
    - Types that are box instances, indicated by `zaf::IsBoxedInstanceTypeV<T>` being true.
    - Types that are `std::optional<E>` where `E` supports boxing and unboxing, indicated by 
      `zaf::HasBoxingTraitsV<E>` being true.
    - Types for which custom traits are defined, indicated by
      `zaf::HasCustomPropertyValueTraitsV<T>` being true.
*/
template<typename T, typename = void>
struct PropertyValueTraits { };

/** @cond */
template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<HasBoxingTraitsV<T>>> {

    using ValueType = std::decay_t<T>;
    using BoxedType = typename BoxingTraits<T>::BoxedType;

    static std::shared_ptr<Object> ToBoxedObject(T&& value) {
        return zaf::Box(std::forward<T>(value));
    }

    static const ValueType& FromBoxedObject(const std::shared_ptr<Object>& object) {
        return zaf::Unbox<ValueType>(*object);
    }
};

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<IsBoxedInstanceTypeV<T>>> {

    using ValueType = std::decay_t<T>;
    using BoxedType = typename ValueType::element_type;

    static std::shared_ptr<Object> ToBoxedObject(T&& value) {
        return std::forward<T>(value);
    }

    static T FromBoxedObject(const std::shared_ptr<Object>& object) {
        return As<typename ValueType::element_type>(object);
    }
};

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<
    IsOptionalV<std::decay_t<T>> && 
    HasBoxingTraitsV<GetOptionalValueTypeT<std::decay_t<T>>>>> {

private:
    using OptionalValueType = GetOptionalValueTypeT<std::decay_t<T>>;

public:
    using ValueType = std::decay_t<T>;
    using BoxedType = typename BoxingTraits<OptionalValueType>::BoxedType;

    static std::shared_ptr<Object> ToBoxedObject(T&& value) {
        if (value.has_value()) {
            return zaf::Box(std::forward<OptionalValueType>(*value));
        }
        return nullptr;
    }

    static ValueType FromBoxedObject(const std::shared_ptr<Object>& object) {
        if (object) {
            return zaf::Unbox<OptionalValueType>(*object);
        }
        else {
            return std::nullopt;
        }
    }
};

template<typename T>
struct PropertyValueTraits<T, std::enable_if_t<HasCustomPropertyValueTraitsV<std::decay_t<T>>>> {
private:
    using CustomTraits = zaf__CustomPropertyValueTraits<std::decay_t<T>>;

public:
    using ValueType = std::decay_t<T>;
    using BoxedType = typename CustomTraits::BoxedType;

    static std::shared_ptr<Object> ToBoxedObject(T&& value) {
        return CustomTraits::ToBoxedObject(std::forward<T>(value));
    }

    static T FromBoxedObject(const std::shared_ptr<Object>& object) {
        auto custom_boxed_object = As<BoxedType>(object);
        if (custom_boxed_object) {
            return CustomTraits::FromBoxedObject(custom_boxed_object);
        }
        throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
    }
};
/** @endcond */

}