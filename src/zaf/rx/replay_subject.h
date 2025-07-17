#pragma once

#include <zaf/rx/base_subject.h>

namespace zaf::rx {

template<typename T>
class ReplaySubject : public BaseSubject<T> {
public:
    ReplaySubject() : BaseSubject<T>(internal::CreateReplaySubjectCore(std::nullopt)) {

    }

    explicit ReplaySubject(std::size_t replay_size) : 
        BaseSubject<T>(internal::CreateReplaySubjectCore(replay_size)) {

    }
};

}