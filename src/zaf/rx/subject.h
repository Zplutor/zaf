#pragma once

#include <zaf/rx/internal/subject/inner_subject.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf {

template<typename T>
class Subject {
public:
    Subject() : inner_(std::make_unique<internal::InnerSubject>()) { }

    Subject(const Subject&) = delete;
    Subject& operator=(const Subject&) = delete;

    Subject(Subject&&) = default;
    Subject& operator=(Subject&&) = default;

    Observable<T> GetObservable() const {
        return Observable<T>(inner_->GetObservable());
    }

    Observer<T> GetObserver() const {
        return Observer<T>(inner_->GetObserver());
    }

private:
    std::unique_ptr<internal::InnerSubject> inner_;
};

}