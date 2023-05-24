#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subject/inner_subject_indirect.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf {

template<typename T>
class Subject : NonCopyable {
public:
    Subject() : inner_(internal::CreateInnerSubject()) { }

    Observable<T> GetObservable() const {
        return Observable<T>{ internal::GetObservableFromInnerSubject(inner_) };
    }

    Observer<T> GetObserver() const {
        return Observer<T>{ internal::GetObserverFromInnerSubject(inner_) };
    }

private:
    std::shared_ptr<internal::InnerSubject> inner_;
};

}