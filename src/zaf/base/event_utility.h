#pragma once

#include <memory>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/subject.h>
#include <zaf/serialization/property_map.h>

namespace zaf {

template<typename T>
Observable<T> GetEventObservable(PropertyMap& property_map, const std::wstring& property_name) {

    auto subject = property_map.GetProperty<std::shared_ptr<Subject<T>>>(
        property_name,
        []() {
            return std::make_shared<Subject<T>>();
        }
    );

    return subject->GetObservable();
}


template<typename T>
std::optional<Observer<T>> GetEventObserver(
    const PropertyMap& property_map, 
    const std::wstring& property_name) {

    auto subject = property_map.TryGetProperty<std::shared_ptr<Subject<T>>>(property_name);
    if (subject) {
        return (*subject)->GetObserver();
    }
    return std::nullopt;
}

}