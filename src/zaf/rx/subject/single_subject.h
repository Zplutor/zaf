#pragma once

#include <zaf/rx/internal/insider/single_insider.h>
#include <zaf/rx/internal/insider/single_observer_insider.h>
#include <zaf/rx/single.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subject/subject_base.h>

namespace zaf::rx {

template<typename T>
class SingleSubject : public SubjectBase<T> {
public:
    SingleSubject() : SubjectBase<T>(internal::CreateReplaySubjectCore(1)) {

    }

    Single<T> AsSingle() const noexcept {
        auto observable_core = internal::AsObservableCore(this->Core());
        return internal::SingleInsider::Create<T>(std::move(observable_core));
    }

    SingleObserver<T> AsObserver() const noexcept {
        auto observer_core = internal::AsObserverCore(this->Core());
        return internal::SingleObserverInsider::Create<T>(std::move(observer_core));
    }
};

}