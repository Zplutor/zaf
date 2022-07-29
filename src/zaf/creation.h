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
 Create a smart pointer object of specified type.

 T is the type of object to create, and ArgumentTypes is the arguments passed 
 to its constructor. T can be any types. 
 
 You should always use this method to create windows and controls for proper initialization.
 */
template<typename T, typename... ArgumentTypes>
std::shared_ptr<T> Create(ArgumentTypes&&... arguments) {

    auto result = std::make_shared<T>(std::forward<ArgumentTypes>(arguments)...);
    internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    return result;
}


template<typename T> 
std::shared_ptr<T> Create(T* object) {

    std::shared_ptr<T> result(object);
    internal::ObjectInitializerSelector<T>::Type::Initialize(result.get());
    return result;
}

}