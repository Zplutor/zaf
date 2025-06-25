#include <zaf/rx/internal/subject/inner_subject_indirect.h>
#include <zaf/rx/internal/subject/inner_subject.h>
#include <zaf/rx/internal/subject/replay_subject_core.h>

namespace zaf::internal {

std::shared_ptr<InnerSubject> CreateInnerSubject() {
    return std::make_shared<InnerSubject>();
}


std::shared_ptr<InnerSubject> CreateReplaySubjectCore(std::optional<std::size_t> replay_size) {
    return std::make_shared<zaf::rx::internal::ReplaySubjectCore>(replay_size);
}


std::shared_ptr<ObservableCore> GetObservableFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner) {

    return inner;
}


std::shared_ptr<ObserverCore> GetObserverFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner) {

    return inner;
}

}