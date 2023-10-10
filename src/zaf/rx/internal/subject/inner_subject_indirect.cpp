#include <zaf/rx/internal/subject/inner_subject_indirect.h>
#include <zaf/rx/internal/subject/inner_replay_subject.h>
#include <zaf/rx/internal/subject/inner_subject.h>

namespace zaf::internal {

std::shared_ptr<InnerSubject> CreateInnerSubject() {
    return std::make_shared<InnerSubject>();
}


std::shared_ptr<InnerSubject> CreateInnerReplaySubject() {
    return std::make_shared<InnerReplaySubject>();
}


std::shared_ptr<InnerObservable> GetObservableFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner) {

    return inner;
}


std::shared_ptr<InnerObserver> GetObserverFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner) {

    return inner;
}

}