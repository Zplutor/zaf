#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf::rx::internal {

template<typename T> 
class BaseSubject : NonCopyable {
public:
    /**
    This class is not used for polymorphism purpose, so there is no virtual destructor.
    */
    ~BaseSubject() = default;

    BaseSubject(BaseSubject&&) = default;
    BaseSubject& operator=(BaseSubject&&) = default;

    Observable<T> AsObservable() const noexcept {
        return ObservableFactory<T>::CreateObservable(internal::AsObservableCore(core_));
    }

    Observer<T> AsObserver() const noexcept {
        return ObservableFactory<T>::CreateObserver(internal::AsObserverCore(core_));
    }

    const std::shared_ptr<internal::SubjectCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit BaseSubject(std::shared_ptr<internal::SubjectCore> core) noexcept : 
        core_(std::move(core)) { }

private:
    std::shared_ptr<internal::SubjectCore> core_;
};

}