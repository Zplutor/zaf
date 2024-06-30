#pragma once

#include <type_traits>

namespace zaf {

template<typename DecayedT>
struct CustomPropertyValueTraits {
    //BoxedType
    //Box
    //Unbox
};


template<typename DecayedT, typename = void>
struct HasCustomPropertyValueTraits : std::false_type { };

template<typename DecayedT>
struct HasCustomPropertyValueTraits<DecayedT,
    std::void_t<typename CustomPropertyValueTraits<DecayedT>::BoxedType>> :
    std::true_type { };

template<typename DecayedT>
constexpr bool HasCustomPropertyValueTraitsV = HasCustomPropertyValueTraits<DecayedT>::value;

}