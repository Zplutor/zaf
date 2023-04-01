#pragma once

#include <memory>

/**
Here are indirect access functions to SubjectCore, to avoid implicit dependencies to internal 
headers.
*/

namespace zaf::internal {

class InnerObservable;
class InnerObserver;
class SubjectCore;

std::shared_ptr<SubjectCore> CreateSubjectCore();

std::shared_ptr<InnerObservable> GetObservableFromSubjectCore(
    const std::shared_ptr<SubjectCore>& core);

std::shared_ptr<InnerObserver> GetObserverFromSubjectCore(
    const std::shared_ptr<SubjectCore>& core);

}