#pragma once

#include <memory>

/**
Here are indirect access functions to InnerSubject, to avoid implicit dependencies to internal 
headers.
*/

namespace zaf::internal {

class InnerObservable;
class InnerObserver;
class InnerSubject;

std::shared_ptr<InnerSubject> CreateInnerSubject();
std::shared_ptr<InnerSubject> CreateReplaySubjectCore(std::optional<std::size_t> replay_size);

std::shared_ptr<InnerObservable> GetObservableFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner);

std::shared_ptr<InnerObserver> GetObserverFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner);

}