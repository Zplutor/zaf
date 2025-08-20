#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject/subject.h>

namespace zaf {

template<typename T>
class Event : NonCopyable {
public:
    rx::Observable<T> GetObservable() const {
        if (!subject_) {
            subject_ = rx::Subject<T>{};
        }
        return subject_->AsObservable();
    }

    void Raise(const T& event_info) const {
        if (subject_) {
            subject_->AsObserver().OnNext(event_info);
        }
    }

private:
    mutable std::optional<rx::Subject<T>> subject_;
};

}