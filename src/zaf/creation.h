#pragma once

#include <memory>
#include <type_traits>
#include <zaf/reflection/internal/reflection_object_initializer.h>

namespace zaf {

class ReflectionObject;

namespace internal {

template<typename T>
struct ReflectionObjectCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<T> Create(ArgumentTypes&&... arguments) {

        auto object = std::make_shared<T>(std::forward<ArgumentTypes>(arguments)...);
        ReflectionObjectInitializer::Initialize(*object);
        return object;
    }
};


template<typename GenericType>
struct GenericCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<GenericType> Create(ArgumentTypes&&... arguments) {
        return std::make_shared<GenericType>(std::forward<ArgumentTypes>(arguments)...);
    }
};


template<typename T>
struct Creator {

    typedef typename std::conditional<
        std::is_base_of<ReflectionObject, T>::value, 
        ReflectionObjectCreator<T>,
        GenericCreator<T>
    >::type Type;
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
    return internal::Creator<T>::Type::Create(std::forward<ArgumentTypes>(arguments)...);
}

}