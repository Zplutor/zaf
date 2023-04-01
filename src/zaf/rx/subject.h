#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf {

template<typename T>
class Subject : NonCopyable {
public:
    Subject() : core_(internal::CreateSubjectCore()) { }

    Observable<T> GetObservable() const {
        return Observable<T>{ internal::GetObservableFromSubjectCore(core_) };
    }

    Observer<T> GetObserver() const {
        return Observer<T>{ internal::GetObserverFromSubjectCore(core_) };
    }

private:
    std::shared_ptr<internal::SubjectCore> core_;
};

}