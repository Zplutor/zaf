#pragma once

#include <memory>

/**
Here are indirect access functions to InnerSubject, to avoid implicit dependencies to internal 
headers.
*/

namespace zaf::internal {

class ObservableCore;
class ObserverCore;
class InnerSubject;

std::shared_ptr<InnerSubject> CreateInnerSubject();
std::shared_ptr<InnerSubject> CreateReplaySubjectCore(std::optional<std::size_t> replay_size);

std::shared_ptr<ObservableCore> GetObservableFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner);

std::shared_ptr<ObserverCore> GetObserverFromInnerSubject(
    const std::shared_ptr<InnerSubject>& inner);

}