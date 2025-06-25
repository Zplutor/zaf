#pragma once

#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/single.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/internal/single_factory.h>

namespace zaf::rx {

template<typename T>
class SingleSubject {
public:
    static SingleSubject Create() {
        return SingleSubject{ zaf::internal::CreateReplaySubjectCore(1) };
    }

public:
    Single<T> AsSingle() const noexcept {
        auto observable_core = zaf::internal::AsObservableCore(core_);
        return internal::SingleFactory<T>::Create(std::move(observable_core));
    }

    SingleObserver<T> AsObserver() noexcept {
        return SingleObserver<T>{ zaf::internal::AsObserverCore(core_) };
    }

private:
    explicit SingleSubject(std::shared_ptr<zaf::internal::SubjectCore> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<zaf::internal::SubjectCore> core_;
};

}