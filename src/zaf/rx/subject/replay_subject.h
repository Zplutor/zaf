#pragma once

#include <zaf/rx/subject/subject_base.h>

namespace zaf::rx {

template<typename T>
class ReplaySubject : public SubjectBase<T> {
public:
    ReplaySubject() : SubjectBase<T>(internal::CreateReplaySubjectCore(std::nullopt)) {

    }

    explicit ReplaySubject(std::size_t replay_size) : 
        SubjectBase<T>(internal::CreateReplaySubjectCore(replay_size)) {

    }
};

}