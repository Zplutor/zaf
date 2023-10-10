#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subject/inner_subject_indirect.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf::internal {

template<typename T> 
class BaseSubject : NonCopyable {
public:
    /**
    This class is not used for polymorphism purpose, so there is no virtual destructor.
    */
    ~BaseSubject() = default;

    BaseSubject(BaseSubject&&) = default;
    BaseSubject& operator=(BaseSubject&&) = default;

    Observable<T> GetObservable() const {
        return Observable<T>{ internal::GetObservableFromInnerSubject(inner_) };
    }

    Observer<T> GetObserver() const {
        return Observer<T>{ internal::GetObserverFromInnerSubject(inner_) };
    }

protected:
    explicit BaseSubject(std::shared_ptr<internal::InnerSubject> inner) : 
        inner_(std::move(inner)) { }

private:
    std::shared_ptr<internal::InnerSubject> inner_;
};

}