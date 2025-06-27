#pragma once

#include <zaf/rx/internal/single_factory.h>
#include <zaf/rx/internal/subject/base_subject.h>
#include <zaf/rx/single.h>
#include <zaf/rx/single_observer.h>

namespace zaf::rx {

template<typename T>
class SingleSubject : public internal::BaseSubject<T> {
public:
    SingleSubject() : internal::BaseSubject<T>(internal::CreateReplaySubjectCore(1)) {

    }

    Single<T> AsSingle() const noexcept {
        auto observable_core = internal::AsObservableCore(this->Core());
        return internal::SingleFactory<T>::Create(std::move(observable_core));
    }

    SingleObserver<T> AsObserver() const noexcept {
        return SingleObserver<T>{ internal::AsObserverCore(this->Core()) };
    }
};

}