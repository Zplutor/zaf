#pragma once

#include <memory>
#include <zaf/serialization/property_map.h>

namespace zaf {

template<typename EventType>
const std::shared_ptr<EventType> TryGetEventFromPropertyMap(
    const PropertyMap& property_map, 
    const std::wstring& property_name
) {

    auto event = property_map.TryGetProperty<std::shared_ptr<EventType>>(property_name);
    if (event != nullptr) {
        return *event;
    }
    else {
        return nullptr;
    }
}


template<typename EventType>
typename EventType::Proxy GetEventProxyFromPropertyMap(
    PropertyMap& property_map, 
    const std::wstring& property_name
) {

    auto event = property_map.GetProperty<std::shared_ptr<EventType>>(
        property_name, 
        []() { return std::make_shared<EventType>(); }
    );

    return typename EventType::Proxy(*event);
}

}