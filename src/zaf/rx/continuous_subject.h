#pragma once

#include <zaf/rx/continuous.h>
#include <zaf/rx/internal/continuous_factory.h>
#include <zaf/rx/internal/subject/base_subject.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>

namespace zaf::rx {

template<typename T>
class ContinuousSubject : public internal::BaseSubject<T> {
public:
    ContinuousSubject() : internal::BaseSubject<T>(internal::CreateSubjectCore()) {

    }

    Continuous<T> AsContinuous() const noexcept {
        auto observable_core = internal::AsObservableCore(this->Core());
        return internal::ContinuousFactory<T>::CreateContinuous(std::move(observable_core));
    }

    ContinuousObserver<T> AsObserver() const noexcept {
        auto observer_core = internal::AsObserverCore(this->Core());
        return internal::ContinuousFactory<T>::CreateContinuousObserver(std::move(observer_core));
    }
};

}