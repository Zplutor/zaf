#pragma once

#include <memory>

namespace zaf::internal {

class InnerObservable;
class InnerObserver;
class SubjectCore;

class InnerSubject {
public:
    InnerSubject();
    ~InnerSubject();

    InnerSubject(const InnerSubject&) = delete;
    InnerSubject& operator=(const InnerSubject&) = delete;

    std::shared_ptr<InnerObservable> GetObservable() const;
    std::shared_ptr<InnerObserver> GetObserver() const;

private:
    std::shared_ptr<SubjectCore> core_;
};

}