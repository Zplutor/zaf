#pragma once

#include <zaf/rx/internal/subject/subject_implementation.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/observer.h>

namespace zaf {

template<typename T>
class Subject {
public:
    Subject() : implementation_(std::make_unique<internal::SubjectImplementation>()) { }

    Subject(const Subject&) = delete;
    Subject& operator=(const Subject&) = delete;

    Subject(Subject&&) = default;
    Subject& operator=(Subject&&) = default;

    Observable<T> GetObservable() const {
        return Observable<T>(implementation_->GetObservable());
    }

    Observer<T> GetObserver() const {
        return Observer<T>(implementation_->GetObserver());
    }

private:
    std::unique_ptr<internal::SubjectImplementation> implementation_;
};

}