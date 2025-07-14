#pragma once

#include <zaf/rx/observable.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<typename T, typename C>
Observable<T> Concat(const C& container) {
    internal::ObservableList observables;
    for (const auto& each_observable : container) {
        observables.push_back(each_observable.Core());
    }
    auto core = std::make_shared<internal::ConcatObservable>(std::move(observables));
    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


template<typename T>
Observable<T> Concat(std::initializer_list<Observable<T>> observables) {
    return Concat<T, std::initializer_list<Observable<T>>>(observables);
}

}