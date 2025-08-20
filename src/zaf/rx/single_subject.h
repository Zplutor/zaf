#pragma once

#include <zaf/rx/internal/single_factory.h>
#include <zaf/rx/single.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subject_base.h>

namespace zaf::rx {

template<typename T>
class SingleSubject : public SubjectBase<T> {
public:
    SingleSubject() : SubjectBase<T>(internal::CreateReplaySubjectCore(1)) {

    }

    Single<T> AsSingle() const noexcept {
        auto observable_core = internal::AsObservableCore(this->Core());
        return internal::SingleFactory<T>::CreateSingle(std::move(observable_core));
    }

    SingleObserver<T> AsObserver() const noexcept {
        auto observer_core = internal::AsObserverCore(this->Core());
        return internal::SingleFactory<T>::CreateSingleObserver(std::move(observer_core));
    }
};

}