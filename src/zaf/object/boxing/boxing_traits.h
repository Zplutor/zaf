#pragma once

#include <zaf/base/as.h>
#include <zaf/object/boxing/boxed_type.h>
#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/reflective_type.h>

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


template<typename T, typename = void>
struct BoxingTraits { };


template<typename T>
struct BoxingTraits<T, std::enable_if_t<IsReflectiveTypeV<T>>> {

    using BoxedType = std::decay_t<T>;
    using UnboxedType = BoxedType; //The same as boxed type.
    using InstanceType = std::shared_ptr<BoxedType>;

    static InstanceType Box(T value) {
        return std::make_shared<BoxedType>(std::move(value));
    }

    static const UnboxedType* Unbox(const Object& object) {
        return As<BoxedType>(&object);
    }
};


template<typename T>
struct BoxingTraits<T, std::enable_if_t<HasCustomBoxingTraitsV<std::decay_t<T>>>> {

    using UnboxedType = std::decay_t<T>;
    using BoxedType = typename CustomBoxingTraits<UnboxedType>::BoxedType;
    using InstanceType = std::shared_ptr<BoxedType>;

    static InstanceType Box(T value) {
        return CustomBoxingTraits<UnboxedType>::Box(std::move(value));
    }

    static const UnboxedType* Unbox(const Object& object) {
        auto boxed_object = As<BoxedType>(&object);
        if (boxed_object) {
            return CustomBoxingTraits<UnboxedType>::Unbox(*boxed_object);
        }
        return nullptr;
    }
};

}