#include <zaf/rx/internal/subject/replay_subject_core.h>

namespace zaf::rx::internal {

ReplaySubjectCore::ReplaySubjectCore(std::optional<std::size_t> replay_size) noexcept :
    replay_size_(replay_size) {

}


std::shared_ptr<SubscriptionCore> ReplaySubjectCore::Subscribe(ObserverShim&& observer) {

    std::deque<std::any> copied_replay_values;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_replay_values = replay_values_;
    }

    //This instance may be destroyed during the emissions.
    //Keep it alive here.
    auto shared_this = shared_from_this();

    for (const auto& each_value : copied_replay_values) {
        observer.OnNext(each_value);
    }

    return __super::Subscribe(std::move(observer));
}


void ReplaySubjectCore::OnNext(const std::any& value) {

    if (!TryEmitOnNext(value)) {
        return;
    }

    if (replay_size_ == 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(lock_);
    if (replay_values_.size() == replay_size_) {
        replay_values_.pop_front();
    }
    replay_values_.push_back(value);
}

}