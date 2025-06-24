#pragma once

#include <zaf/rx/single.h>
#include <zaf/rx/single_observer.h>

namespace zaf::rx::internal {
class SingleSubjectCore;
}

namespace zaf::rx {

template<typename T>
class SingleSubject {
public:
    static SingleSubject Create() {
        return SingleSubject{ std::make_shared<internal::SingleSubjectCore>() };
    }

public:
    Single<T> AsSingle() const noexcept {
        return Single<T>{ core_ };
    }

    SingleObserver<T> AsObserver() noexcept {
        return SingleObserver<T>{ core_ };
    }

private:
    explicit SingleSubject(std::shared_ptr<internal::SingleSubjectCore> core) noexcept : 
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::SingleSubjectCore> core_;
};

}