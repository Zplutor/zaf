#pragma once

#include <type_traits>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/boxing/custom_boxed_type_utility.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/reflective_type_utility.h>

namespace zaf {
namespace internal {

template<typename T>
struct IsSharedPtrImpl {
    static constexpr bool Value = false;
};

template<typename E>
struct IsSharedPtrImpl<std::shared_ptr<E>> {
    static constexpr bool Value = true;
};

template<typename T>
struct GetSharedPtrElementTypeImpl {
    using Type = void;
};

template<typename E>
struct GetSharedPtrElementTypeImpl<std::shared_ptr<E>> {
    using Type = E;
};

}


template<typename T>
struct ToBoxedType {
private:
    using DecayType = std::decay_t<T>;

    struct ReflectiveTypeSelector {
        using type = DecayType;
    };

    struct CustomTypeSelector {
        using type = typename GetCustomBoxedType<DecayType>::type;
    };

    using Selector = std::conditional_t<IsReflectiveTypeV<DecayType>, ReflectiveTypeSelector, CustomTypeSelector>;

public:
    using type = typename Selector::type;
};

template<typename T>
using ToBoxedTypeT = typename ToBoxedType<T>::type;


template<typename T>
struct ToBoxedInstanceType {
private:
    using DecayType = std::decay_t<T>;

    struct SharedPtrConverter {
        using Type = ToBoxedTypeT<
            typename internal::GetSharedPtrElementTypeImpl<DecayType>::Type
        >;
    };

    struct NonSharedPtrConverter {
        using Type = ToBoxedTypeT<DecayType>;
    };

    using Converter = std::conditional_t<
        internal::IsSharedPtrImpl<DecayType>::Value,
        SharedPtrConverter,
        NonSharedPtrConverter
    >;

    using BoxedType = typename Converter::Type;

public:
    using type = std::shared_ptr<BoxedType>;
};

template<typename T>
using ToBoxedInstanceTypeT = typename ToBoxedInstanceType<T>::type;

}