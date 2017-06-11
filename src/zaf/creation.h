#pragma once

#include <memory>
#include <type_traits>
#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf {
namespace internal {

template<typename WindowType>
struct WindowCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<WindowType> Create(ArgumentTypes&&... arguments) {

        auto window = std::make_shared<WindowType>(std::forward<ArgumentTypes>(arguments)...);
        window->Initialize();
        return window;
    }
};


template<typename ControlType>
struct ControlCreator {

    template<typename... ArgumentTypes>
    static std::shared_ptr<ControlType> Create(ArgumentTypes&&... arguments) {

        auto control = std::make_shared<ControlType>(std::forward<ArgumentTypes>(arguments)...);
        Control::UpdateGuard update_guard(*control);
        control->Initialize();
        return control;
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
        std::is_base_of<Window, ObjectType>::value, 
        WindowCreator<ObjectType>, 
        typename std::conditional<
            std::is_base_of<Control, ObjectType>::value, 
            ControlCreator<ObjectType>,
            GenericCreator<ObjectType>
        >::type
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