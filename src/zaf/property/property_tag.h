#pragma once

#include <zaf/base/event.h>

namespace zaf {
namespace internal {

template<typename SetterType, typename ParameterType>
struct PropertyTag {
    SetterType operator=(const ParameterType& parameter) const {
        return SetterType{ parameter };
    }
};


template<typename SetterType, typename EventType>
struct EventPropertyTag {
    SetterType operator=(const typename EventType::ListenerType& listener) const {
        return SetterType{ false, 0, listener };
    }

    SetterType operator=(const std::pair<std::uintptr_t, typename EventType::ListenerType>& listener) const {
        return SetterType{ true, listener.first, listener.second };
    }
};

}

#define ZAF_DEFINE_PROPERTY_TAG(Name, ValueType, ObjectType, Setter)                    \
struct Name##PropertySetter {                                                           \
    ValueType value;                                                                    \
    void operator()(ObjectType& object) {                                               \
        object.Setter(value);                                                           \
    }                                                                                   \
};                                                                                      \
typedef zaf::internal::PropertyTag<Name##PropertySetter, ValueType> Name##PropertyTag;  \
const Name##PropertyTag Name;                   


#define ZAF_DEFINE_EVENT_PROPERTY_TAG(Name, EventType, ObjectType, EventGetter)                        \
struct Name##EventPropertySetter {                                                                     \
    bool has_tag;                                                                                      \
    std::uintptr_t tag;                                                                                \
    EventType::ListenerType listener;                                                                  \
    void operator()(ObjectType& object) {                                                              \
        if (has_tag) {                                                                                 \
            object.EventGetter().AddListenerWithTag(tag, listener);                                    \
        }                                                                                              \
        else {                                                                                         \
            object.EventGetter().AddListener(listener);                                                \
        }                                                                                              \
    }                                                                                                  \
};                                                                                                     \
typedef zaf::internal::EventPropertyTag<Name##EventPropertySetter, EventType> Name##EventPropertyTag;  \
const Name##EventPropertyTag Name;

}