#pragma once

#include <zaf/object/boxed.h>
#include <zaf/object/internal/built_in_box_types.h>
#include <zaf/object/numeric.h>
#include <zaf/object/string.h>

namespace zaf::internal {

template<typename T>
struct GetGeneralBoxType {

    using Type = std::conditional_t<
        HasBuiltInBoxType<T>::Value,
        typename GetBuiltInBoxType<T>::Type,
        Boxed<T>
    >;
};


template<typename T>
struct GetBoxType {
private:
    using DecayType = std::decay_t<T>;

public:
    using Type = std::conditional_t<
        std::is_base_of_v<Object, DecayType>, 
        DecayType,
        typename GetGeneralBoxType<DecayType>::Type
    >;
};

}