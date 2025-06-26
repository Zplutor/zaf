#pragma once

#include <memory>

/**
Here are indirect access functions to SubjectCore, to avoid implicit dependencies to internal 
headers.
*/

namespace zaf::rx::internal {

class ObservableCore;
class ObserverCore;
class SubjectCore;

std::shared_ptr<SubjectCore> CreateSubjectCore();
std::shared_ptr<SubjectCore> CreateReplaySubjectCore(std::optional<std::size_t> replay_size);

std::shared_ptr<ObservableCore> AsObservableCore(const std::shared_ptr<SubjectCore>& core);
std::shared_ptr<ObserverCore> AsObserverCore(const std::shared_ptr<SubjectCore>& core);

}