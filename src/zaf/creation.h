#pragma once

#include <memory>
#include <type_traits>
#include <zaf/object/internal/object_initializer.h>

namespace zaf {

class Object;

namespace internal {

template<typename T>
struct ObjectCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<T> Create(ArgumentTypes&&... arguments) {

        auto object = std::make_shared<T>(std::forward<ArgumentTypes>(arguments)...);
        ObjectInitializer::Initialize(*object);
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
        std::is_base_of<Object, T>::value, 
        ObjectCreator<T>,
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