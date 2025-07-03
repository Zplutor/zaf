#pragma once

#include <zaf/rx/internal/once_factory.h>
#include <zaf/rx/internal/subject/base_subject.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/once.h>
#include <zaf/rx/once_observer.h>

namespace zaf::rx {

template<typename T>
class OnceSubject : public internal::BaseSubject<T> {
public:
    OnceSubject() : internal::BaseSubject<T>(internal::CreateReplaySubjectCore(1)) {

    }

    Once<T> AsOnce() const noexcept {
        return internal::OnceFactory<T>::CreateOnce(internal::AsObservableCore(this->Core()));
    }

    OnceObserver<T> AsObserver() const noexcept {
        auto observer_core = internal::AsObserverCore(this->Core());
        return internal::OnceFactory<T>::CreateOnceObserver(std::move(observer_core));
    }
};

}