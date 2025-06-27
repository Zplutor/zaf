#pragma once

#include <zaf/rx/internal/subject/base_subject.h>

namespace zaf::rx {

template<typename T>
class ReplaySubject : public internal::BaseSubject<T> {
public:
    ReplaySubject() : internal::BaseSubject<T>(internal::CreateReplaySubjectCore(std::nullopt)) {

    }

    explicit ReplaySubject(std::size_t replay_size) : 
        internal::BaseSubject<T>(internal::CreateReplaySubjectCore(replay_size)) {

    }
};

}