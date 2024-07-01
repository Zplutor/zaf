#pragma once

#include <zaf/base/as.h>
#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/reflective_type.h>

//Built-in custom boxing types should be included to make them available.
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>

namespace zaf {

template<typename T>
struct IsBoxedInstanceType {
private:
    using DecayType = std::decay_t<T>;

public:
    static constexpr bool value =
        internal::IsSharedPtrV<DecayType> &&
        IsReflectiveTypeV<internal::GetSharedPtrElementTypeT<DecayType>>;
};

template<typename T>
constexpr bool IsBoxedInstanceTypeV = IsBoxedInstanceType<T>::value;


/**
A template for getting the corresponding boxed type and performing boxing/unboxing operations for 
the specified non-boxed type.

@tparam T 
    The type for which the boxed type and boxing/unboxing operations are to be defined.

@details
    This template is used by zaf::Box() and zaf::Unbox() functions to get boxed types and perform 
    boxing/unboxing for non-boxed types. The primary template doesn't define any members and will 
    emit a compile error if initiated, indicating that the specified type doesn't support boxing 
    and unboxing.

    If the specified `T` supports boxing and unboxing, the following members will be defined in 
    this template:

    - A type alias named `UnboxedType` that defines the decayed non-boxed type for `T`. It is equal
      to `std::decay_t<T>`.

    - A type alias named `BoxedType` that defines the decayed boxed type for `T`.

    - A type alias named `BoxedInstanceType` that defines the boxed instance type for `T`. A boxed 
      instance is a std::shared_ptr to a boxed type. It is equal to `std::shared_ptr<BoxedType>`.

    - A static method named `Box` that boxes a non-boxed value to a boxed object. The signature of
      the method is:

      @code
      BoxedInstanceType Box(T value);
      @endcode

      Instances of `T` should be movable to enable boxing operation.

    - A static method named `Unbox` that unboxes a boxed object to a non-boxed value. The signature 
      of the method is:

      @code
      const UnboxedType* Unbox(const Object& object);
      @endcode

      Note that the returned pointer points to the inner member of the boxed object.

    This template uses zaf__CustomBoxingTraits to define these members for non-reflective types.
*/
template<typename T, typename = void>
struct BoxingTraits { };

/** @cond */
template<typename T>
struct BoxingTraits<T, std::enable_if_t<IsReflectiveTypeV<T>>> {

    using BoxedType = std::decay_t<T>;
    using UnboxedType = BoxedType; //The same as boxed type.
    using BoxedInstanceType = std::shared_ptr<BoxedType>;

    static BoxedInstanceType Box(T value) {
        return std::make_shared<BoxedType>(std::move(value));
    }

    static const UnboxedType* Unbox(const Object& object) {
        return As<BoxedType>(&object);
    }
};

template<typename T>
struct BoxingTraits<T, std::enable_if_t<HasCustomBoxingTraitsV<std::decay_t<T>>>> {

    using UnboxedType = std::decay_t<T>;
    using BoxedType = typename zaf__CustomBoxingTraits<UnboxedType>::BoxedType;
    using BoxedInstanceType = std::shared_ptr<BoxedType>;

    static BoxedInstanceType Box(T value) {
        return zaf__CustomBoxingTraits<UnboxedType>::Box(std::move(value));
    }

    static const UnboxedType* Unbox(const Object& object) {
        auto boxed_object = As<BoxedType>(&object);
        if (boxed_object) {
            return zaf__CustomBoxingTraits<UnboxedType>::Unbox(*boxed_object);
        }
        return nullptr;
    }
};
/** @endcond */


/**
A template to check if there is a valid definition of BoxingTraits for the specified type;
in other words, if the specified type supports boxing/unboxing.

@tparam T
    The type to check.
*/
template<typename T, typename = void>
struct HasBoxingTraits : std::false_type { };

/** @cond */
template<typename T>
struct HasBoxingTraits<T, std::void_t<typename BoxingTraits<T>::BoxedType>> : std::true_type { };
/** @endcond */


/**
A helper variable template for HasBoxingTraits.

@relates HasBoxingTraits
*/
template<typename T>
constexpr bool HasBoxingTraitsV = HasBoxingTraits<T>::value;

}