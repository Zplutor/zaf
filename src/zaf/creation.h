#pragma once

#include <memory>
#include <type_traits>
#include <zaf/object/internal/object_initializer.h>

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
Creates an object of specified type managed by smart pointer.

T is the type of object to be created, and ArgumentTypes is the arguments passed 
to its constructor.

T can be any types. If T is a derived class of zaf::Object, the object would be initialized 
before returned. Otherwise this function is the same as std::make_shared.
 
Typically, derived classes of zaf::Object should be created using this function in order to get 
initialized properly, especially windows and controls.
*/
template<typename T, typename... ArgumentTypes>
std::shared_ptr<T> Create(ArgumentTypes&&... arguments) {

    auto result = std::make_shared<T>(std::forward<ArgumentTypes>(arguments)...);
    internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    return result;
}


/**
Initializes a dynamic allocated object of specified type, and turns it to smart pointer.

T is the type of object, it can be any types. If T is a derived class of zaf::Object, the object 
would be initialized. Otherwise this function just turns the raw pointer to smart pointer.

The function is used when zaf::Create() is unavaliable, such as when T's constructor is private.
*/
template<typename T> 
std::shared_ptr<T> Init(T* object) {

    std::shared_ptr<T> result(object);
    internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    return result;
}

}