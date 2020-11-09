#include <zaf/rx/internal/subject/subject_implementation.h>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::internal {

SubjectImplementation::SubjectImplementation() : core_(std::make_shared<SubjectCore>()) {

}


SubjectImplementation::~SubjectImplementation() {

}


std::shared_ptr<ObservableImplementation> SubjectImplementation::GetObservable() const {
    return core_;
}


std::shared_ptr<ObserverImplementation> SubjectImplementation::GetObserver() const {
    return core_;
}

}