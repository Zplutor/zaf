#pragma once

/**
@file
    Defines type utilities related to the custom boxing mechanism.
*/

#include <type_traits>

namespace zaf {

/**
A template for defining custom boxed types and corresponding boxing/unboxing operations for
non-boxed types.

@tparam T
    The non-boxed type for which a custom boxed type is to be defined. It should be a decayed type.

@details
    This template is used by zaf::Box() and zaf::Unbox() functions to get boxed types and perform 
    boxing/unboxing for non-boxed types. The primary template doesn't define any members and will 
    emit a compile error if initiated, indicating that the specified type doesn't support boxing 
    and unboxing.

    To support boxing and unboxing for a specified non-boxed type, users should define a 
    specialization with the following members:
    - A type alias named `BoxedType` that specifies the boxed type for the specified non-boxed 
      type.
    - A static method named `Box` that is used to box a non-boxed value to a boxed object. The 
      signature of the method should be:
      @code
      std::shared_ptr<BoxedType> Box(T value);
      @endcode
    - A static method named `Unbox` that is used to unbox a boxed object to a non-boxed value. The
      signature of the method should be:
      @code
      const T* Unbox(const BoxedType& object);
      @endcode
      Note that the Unbox() method should return a pointer to the inner member of the boxed object.

    Example for defining a specialization:
    @code
    //The non-boxed type.
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

    //Define the specialization of CustomBoxingTraits.
    namespace zaf {
        template<>
        struct CustomBoxingTraits<MyNonBoxedValue> {

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
    }
    @endcode
*/
template<typename T>
struct CustomBoxingTraits {
    static_assert(
        std::is_same_v<T, std::decay_t<T>>, 
        "The template argument T of zaf::CustomBoxingTraits<> should be a decayed type.");
};


/**
A template to check if a CustomBoxingTraits specialization is defined for the specified non-boxed
type.

@tparam T 
    The non-boxed type to check. It should be a decayed type.
*/
template<typename T, typename = void>
struct HasCustomBoxingTraits : std::false_type { };

template<typename T>
struct HasCustomBoxingTraits<T, std::void_t<typename CustomBoxingTraits<T>::BoxedType>> : 
    std::true_type { };

/**
A helper variable template for HasCustomBoxingTraits.

@relates HasCustomBoxingTraits
*/
template<typename T>
constexpr bool HasCustomBoxingTraitsV = HasCustomBoxingTraits<T>::value;

}