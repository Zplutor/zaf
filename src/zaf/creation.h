#pragma once

#include <memory>
#include <type_traits>

namespace zaf {
namespace internal {

template<typename WindowType>
struct WindowCreator {

    template<typename... ArgumentTypes>
    static const std::shared_ptr<WindowType> Create(ArgumentTypes&&... arguments) {

        auto window = std::make_shared<WindowType>(arguments...);
        window->Initialize();
        return window;
    }
};


template<typename ControlType>
struct ControlCreator {

    template<typename... ArgumentTypes>
    static const std::shared_ptr<ControlType> Create(ArgumentTypes&&... arguments) {

        auto control = std::make_shared<ControlType>(arguments...);
        Control::UpdateGuard update_guard(*control);
        control->Initialize();
        return control;
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
            void
        >::type
    >::type Type;
};

}

template<typename ObjectType, typename... ArgumentTypes>
const std::shared_ptr<ObjectType> Create(ArgumentTypes&&... arguments) {
    return internal::Creator<ObjectType>::Type::Create(arguments...);
}

}