#pragma once

/**
@file
    Defines type utilities related to the custom property value boxing mechanism.
*/

#include <type_traits>

/**
A template for defining boxed types and boxing/unboxing operations for those property value types 
that are not supported by the property mechanism internally.

@tparam DecayedT
    The property value type for which a boxed type and boxing/unboxing operations are to be 
    defined. It should be a decayed type.

@details
    This template is used by zaf::PropertyValueTraits to get a boxed type and perform 
    boxing/unboxing operation. The primary template doesn't define any members and
    will emit a compile error if initiated, indicating that the specified type does not support 
    being used in properties.

    To enable the specified type to be used in properties, users should define a 
    specialization with the following members:

    - A type alias `BoxedType` that defines the boxed type for `DecayedT`.

    - A static method `ToBoxedObject` that boxes the property value to a boxed object. The 
      signature of the method should be:

      @code
      std::shared_ptr<BoxedType> ToBoxedObject(const DecayedT& value);
      @endcode

    - A static method `FromBoxedObject` that unboxes a boxed object to the property value. The 
      signature of the method should be:

      @code
      DecayedT FromBoxedObject(const std::shared_ptr<BoxedType>& object);
      @endcode
*/
template<typename DecayedT>
struct zaf__CustomPropertyValueTraits { };

namespace zaf {

/**
A template to check if a specialization of zaf__CustomPropertyValueTraits for the specified type is
defined. 

@tparam DecayedT
    The type to check. It should be a decayed type.
*/
template<typename DecayedT, typename = void>
struct HasCustomPropertyValueTraits : std::false_type { };

/** @cond */
template<typename DecayedT>
struct HasCustomPropertyValueTraits<DecayedT,
    std::void_t<typename zaf__CustomPropertyValueTraits<DecayedT>::BoxedType>> :
    std::true_type { };
/** @endcond */

/**
A helper template variable for zaf::HasCustomPropertyValueTraits.

@relates zaf::HasCustomPropertyValueTraits
*/
template<typename DecayedT>
constexpr bool HasCustomPropertyValueTraitsV = HasCustomPropertyValueTraits<DecayedT>::value;

}