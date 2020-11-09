#pragma once

#include <memory>

namespace zaf::internal {

class ObservableImplementation;
class ObserverImplementation;
class SubjectCore;

class SubjectImplementation {
public:
    SubjectImplementation();
    ~SubjectImplementation();

    SubjectImplementation(const SubjectImplementation&) = delete;
    SubjectImplementation& operator=(const SubjectImplementation&) = delete;

    std::shared_ptr<ObservableImplementation> GetObservable() const;
    std::shared_ptr<ObserverImplementation> GetObserver() const;

private:
    std::shared_ptr<SubjectCore> core_;
};

}