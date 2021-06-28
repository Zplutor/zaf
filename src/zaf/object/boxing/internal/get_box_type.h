#pragma once

#include <zaf/object/boxing/boxed_object.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
#include <zaf/object/boxing/numeric.h>
#include <zaf/object/boxing/string.h>

namespace zaf::internal {

template<typename T>
struct GetGeneralBoxType {

    using Type = std::conditional_t<
        HasBuiltInBoxType<T>::Value,
        typename GetBuiltInBoxType<T>::Type,
        BoxedObject<T>
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