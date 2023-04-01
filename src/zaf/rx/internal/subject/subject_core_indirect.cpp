#include <zaf/rx/internal/subject/subject_core_indirect.h>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::internal {

std::shared_ptr<SubjectCore> CreateSubjectCore() {
    return std::make_shared<SubjectCore>();
}


std::shared_ptr<InnerObservable> GetObservableFromSubjectCore(
    const std::shared_ptr<SubjectCore>& core) {

    return core;
}


std::shared_ptr<InnerObserver> GetObserverFromSubjectCore(
    const std::shared_ptr<SubjectCore>& core) {

    return core;
}

}