#pragma once

#include <memory>
#include <type_traits>

namespace zaf {

class ReflectionObject;

namespace internal {

template<typename ObjectType>
struct ReflectionObjectCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<ObjectType> Create(ArgumentTypes&&... arguments) {

        auto object = std::make_shared<ObjectType>(std::forward<ArgumentTypes>(arguments)...);
        object->Initialize();
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


template<typename ObjectType>
struct Creator {

    typedef typename std::conditional<
        std::is_base_of<ReflectionObject, ObjectType>::value, 
        ReflectionObjectCreator<ObjectType>,
        GenericCreator<ObjectType>
    >::type Type;
};

}

/**
 Create a smart pointer object of specified type.

 ObjectType is the type of object to create, and ArgumentTypes is the arguments passed 
 to its constructor. ObjectType can be any types. 
 
 You should always use this method to create windows and controls for proper initialization.
 */
template<typename ObjectType, typename... ArgumentTypes>
std::shared_ptr<ObjectType> Create(ArgumentTypes&&... arguments) {
    return internal::Creator<ObjectType>::Type::Create(std::forward<ArgumentTypes>(arguments)...);
}

}