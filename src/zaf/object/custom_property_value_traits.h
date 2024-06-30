#pragma once

#include <type_traits>

template<typename DecayedT>
struct zaf__CustomPropertyValueTraits {
    //BoxedType
    //Box
    //Unbox
};

namespace zaf {

template<typename DecayedT, typename = void>
struct HasCustomPropertyValueTraits : std::false_type { };

template<typename DecayedT>
struct HasCustomPropertyValueTraits<DecayedT,
    std::void_t<typename zaf__CustomPropertyValueTraits<DecayedT>::BoxedType>> :
    std::true_type { };

template<typename DecayedT>
constexpr bool HasCustomPropertyValueTraitsV = HasCustomPropertyValueTraits<DecayedT>::value;

}