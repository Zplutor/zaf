#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/internal/subject/subject_core.h>
#include <zaf/rx/internal/subject/replay_subject_core.h>

namespace zaf::internal {

std::shared_ptr<SubjectCore> CreateSubjectCore() {
    return std::make_shared<SubjectCore>();
}


std::shared_ptr<SubjectCore> CreateReplaySubjectCore(std::optional<std::size_t> replay_size) {
    return std::make_shared<zaf::rx::internal::ReplaySubjectCore>(replay_size);
}


std::shared_ptr<ObservableCore> AsObservableCore(const std::shared_ptr<SubjectCore>& core) {
    return core;
}


std::shared_ptr<ObserverCore> AsObserverCore(const std::shared_ptr<SubjectCore>& core) {
    return core;
}

}