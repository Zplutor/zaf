#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject.h>

namespace zaf {

template<typename T>
class Event : NonCopyable {
public:
    Observable<T> GetObservable() const {
        if (!subject_) {
            subject_ = Subject<T>{};
        }
        return subject_->GetObservable();
    }

    void Raise(const T& event_info) const {
        if (subject_) {
            subject_->GetObserver().OnNext(event_info);
        }
    }

private:
    mutable std::optional<Subject<T>> subject_;
};

}