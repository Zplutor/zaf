#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf::rx {

template<typename T>
class ReplaySubject : public internal::BaseSubject<T> {
public:
    static ReplaySubject Create() {
        return ReplaySubject{ internal::CreateReplaySubjectCore(std::nullopt) };
    }

    static ReplaySubject Create(std::size_t replay_size) {
        return ReplaySubject{ internal::CreateReplaySubjectCore(replay_size) };
    }

private:
    using internal::BaseSubject<T>::BaseSubject;
};

}