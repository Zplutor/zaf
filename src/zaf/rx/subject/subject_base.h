#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/insider/observable_insider.h>
#include <zaf/rx/internal/insider/observer_insider.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf::rx {

template<typename T> 
class SubjectBase : NonCopyable {
public:
    /**
    This class is not used for polymorphism purpose, so there is no virtual destructor.
    */
    ~SubjectBase() = default;

    SubjectBase(SubjectBase&&) = default;
    SubjectBase& operator=(SubjectBase&&) = default;

    Observable<T> AsObservable() const noexcept {
        return internal::ObservableInsider::Create<T>(internal::AsObservableCore(core_));
    }

    Observer<T> AsObserver() const noexcept {
        return internal::ObserverInsider::Create<T>(internal::AsObserverCore(core_));
    }

    const std::shared_ptr<internal::SubjectCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit SubjectBase(std::shared_ptr<internal::SubjectCore> core) noexcept : 
        core_(std::move(core)) { }

private:
    std::shared_ptr<internal::SubjectCore> core_;
};

}