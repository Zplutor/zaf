#pragma once

#include <zaf/object/boxing/boxed_type_utility.h>
#include <zaf/object/custom_property_value_type_utility.h>

namespace zaf {

template<typename T>
struct MakePropertyValueType {
private:
    using DecayType = std::decay_t<T>;

    template<typename K, typename R = ToBoxedInstanceTypeT<K>>
    static constexpr auto TestToBoxInstanceType(int) -> R*;

    template<typename K>
    static constexpr void TestToBoxInstanceType(...);

    using BoxedInstanceType = std::remove_pointer_t<decltype(TestToBoxInstanceType<T>(0))>;

    struct BoxedTypeMaker {
        using type = typename BoxedInstanceType::element_type;
    };

    struct CustomTypeMaker {
        using type = GetCustomPropertyValueTypeT<DecayType>;
    };

    using Maker = std::conditional_t<!std::is_same_v<BoxedInstanceType, void>, BoxedTypeMaker, CustomTypeMaker>;

public:
    using type = typename Maker::type;
};


template<typename T>
using MakePropertyValueTypeT = typename MakePropertyValueType<T>::type;

}