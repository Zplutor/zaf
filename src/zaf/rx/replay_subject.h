#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf::rx {

template<typename T>
class ReplaySubject : public zaf::internal::BaseSubject<T> {
public:
    static ReplaySubject Create() {
        return ReplaySubject{ zaf::internal::CreateReplaySubjectCore(std::nullopt) };
    }

    static ReplaySubject Create(std::size_t replay_size) {
        return ReplaySubject{ zaf::internal::CreateReplaySubjectCore(replay_size) };
    }

private:
    using zaf::internal::BaseSubject<T>::BaseSubject;
};

}