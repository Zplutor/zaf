#pragma once

#include <memory>
#include <zaf/control/control.h>
#include <zaf/creation.h>

namespace zaf {
namespace internal {

template<typename ObjectType>
struct NoneUpdater {
    struct Updater {
        Updater(ObjectType&) { }
    };
};

template<typename ObjectType>
struct UpdaterSelector {
    typedef typename std::conditional<
        std::is_base_of<Control, ObjectType>::value,
        Control::UpdateGuard,
        NoneUpdater<ObjectType>
    >::type Type;
};

template<typename ObjectType, typename SetterType>
void SetProperty(ObjectType& object, SetterType&& setter) {
    setter(object);
}

}

template<typename ObjectType, typename... SetterTypes>
const std::shared_ptr<ObjectType> SetProperties(const std::shared_ptr<ObjectType>& object, SetterTypes&&... setters) {
    internal::UpdaterSelector<ObjectType>::Type updater(*object);
    std::initializer_list<int>{ (internal::SetProperty(*object, setters), 0)... };
    return object;
}


template<typename ObjectType, typename... SetterTypes>
const std::shared_ptr<ObjectType> CreateWithProperties(SetterTypes&&... setters) {
    return SetProperties(Create<ObjectType>(), setters...);
}

}
