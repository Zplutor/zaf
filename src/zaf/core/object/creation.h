#pragma once

/**
@file
    Defines functions to create and initialize objects.
*/

#include <memory>
#include <type_traits>
#include <zaf/core/object/internal/object_initializer.h>

namespace zaf {

class Object;

namespace internal {

class DumbInitializer {
public:
    static void Initialize(void*) { }
};


template<typename T>
class ObjectInitializerSelector {
public:
    using Type = std::conditional_t<
        std::is_base_of_v<Object, T>,
        ObjectInitializer,
        DumbInitializer
    >;
};

}

/**
Creates an object of the specified type managed by a shared pointer, and initializes the object if
it is a derived class of `zaf::Object`.

@tparam T
    The type of the object to be created.
    
@tparam ARGUMENT_TYPES
    The types of the arguments passed to the constructor of the object.

@param arguments
    The arguments passed to the constructor of the object.

@return
    A shared pointer managing the created object.

@throw std::bad_alloc

@throw ...
    Any exception thrown by the constructor of the object, or thrown during the initialization of 
    the object.

@details
    `T` can be any type. If `T` is a derived class of `zaf::Object`, the object will be 
    initialized before returned. Otherwise this function is the same as `std::make_shared`.
 
    Typically, derived classes of `zaf::Object` should be created using this function in order to 
    get initialized properly, especially windows and controls.

@see zaf::Adopt()
*/
template<typename T, typename... ARGUMENT_TYPES>
std::shared_ptr<T> Create(ARGUMENT_TYPES&&... arguments) {

    auto result = std::make_shared<T>(std::forward<ARGUMENT_TYPES>(arguments)...);
    internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    return result;
}


/**
Adopts a dynamic allocated object of the specified type, turning it to a shared pointer, and 
initializes the object if it is a derived class of `zaf::Object`.

@tparam T
    The type of the object to be adopted.

@param object
    The dynamic allocated object to be adopted. The object should be created using `new` operator.

@return
    A shared pointer managing the adopted object. It is null if the `object` is null.

@throw std::bad_alloc

@throw ...
    Any exception thrown during the initialization of the object.

@details
    `T` can be any type. If `T` is a derived class of `zaf::Object`, the object will be initialized
    before returned. Otherwise this function just turns the raw pointer to a shared pointer.

    The function is used when `zaf::Create()` is unavaliable, such as when the constructor is 
    private.

@see zaf::Create()
*/
template<typename T>
std::shared_ptr<T> Adopt(T* object) {

    std::shared_ptr<T> result(object);
    if (result) {
        internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    }
    return result;
}

}