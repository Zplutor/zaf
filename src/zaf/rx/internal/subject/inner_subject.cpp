#include <zaf/rx/internal/subject/inner_subject.h>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::internal {

InnerSubject::InnerSubject() : core_(std::make_shared<SubjectCore>()) {

}


InnerSubject::~InnerSubject() {

}


std::shared_ptr<InnerObservable> InnerSubject::GetObservable() const {
    return core_;
}


std::shared_ptr<InnerObserver> InnerSubject::GetObserver() const {
    return core_;
}

}