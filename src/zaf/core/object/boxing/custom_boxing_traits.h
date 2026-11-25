#pragma once

/**
@file
    Defines type utilities related to the custom boxing mechanism.
*/

#include <type_traits>

/**
A template for defining custom boxed types and corresponding boxing/unboxing operations for 
non-reflective types.

@tparam DecayedT
    The non-reflective type for which a custom boxed type is to be defined. It should be a decayed
    type.

@details
    This template is used by zaf::BoxingTraits to get custom boxed types and perform custom 
    boxing/unboxing for non-reflective types. The primary template doesn't define any members and
    will emit a compile error if initiated, indicating that the specified non-reflective type 
    doesn't support boxing and unboxing.

    To support boxing and unboxing for a specified non-reflective type, users should define a 
    specialization with the following members:

    - A type alias named `BoxedType` that specifies the boxed type for the specified non-reflective 
      type.

    - A static method named `Box` that is used to box a non-boxed value to a boxed object. The 
      signature of the method should be:

      @code
      std::shared_ptr<BoxedType> Box(DecayedT value);
      @endcode

    - A static method named `Unbox` that is used to unbox a boxed object to a non-boxed value. The
      signature of the method should be:

      @code
      const DecayedT* Unbox(const BoxedType& object) noexcept;
      @endcode

      Note that the Unbox() method should return a pointer to the inner member of the boxed object.
      If unboxing fails, the method should return null.

    Example for defining a specialization:
    @code
    //The non-reflective type.
    class MyNonBoxedValue { };

    //The corresponding boxed type.
    class MyBoxedObject : public zaf::Object {
    public:
        ZAF_OBJECT;

        //Constructor to construct the object from a non-boxed value.
        explicit MyBoxedObject(MyNonBoxedValue value) : value_(std::move(value)) {

        }

        //Getter to get the inner non-boxed value.
        const MyNonBoxedValue& Value() const {
            return value_;
        }

    private:
        MyNonBoxedValue value_;
    };

    //Define the specialization of zaf__CustomBoxingTraits.
    template<>
    struct zaf__CustomBoxingTraits<MyNonBoxedValue> {

        //Defines the boxed type.
        using BoxedType = MyBoxedObject;

        //Static method to box the value.
        static std::shared_ptr<MyBoxedObject> Box(MyNonBoxedValue value) {
            return std::make_shared<MyBoxedObject>(std::move(value));
        }

        //Static method to unbox the object.
        static const MyNonBoxedValue* Unbox(const MyBoxedObject& object) {
            return &object.Value();
        }
    };
    @endcode

    This template is defined in the global namespace for convenient specialization in different
    namespaces.
*/
template<typename DecayedT>
struct zaf__CustomBoxingTraits {
    static_assert(
        std::is_same_v<DecayedT, std::decay_t<DecayedT>>,
        "The template argument DecayedT of zaf__CustomBoxingTraits<> should be a decayed type.");
};


namespace zaf {

/**
A template to check if a zaf__CustomBoxingTraits specialization is defined for the specified type.

@tparam DecayedT
    The type to check. It should be a decayed type.
*/
template<typename DecayedT, typename = void>
struct HasCustomBoxingTraits : std::false_type { };

/** @cond */
template<typename DecayedT>
struct HasCustomBoxingTraits<
    DecayedT, 
    std::void_t<typename zaf__CustomBoxingTraits<DecayedT>::BoxedType>
> : std::true_type { };
/** @endcond */

/**
A helper variable template for HasCustomBoxingTraits.

@relates HasCustomBoxingTraits
*/
template<typename DecayedT>
constexpr bool HasCustomBoxingTraitsV = HasCustomBoxingTraits<DecayedT>::value;

}