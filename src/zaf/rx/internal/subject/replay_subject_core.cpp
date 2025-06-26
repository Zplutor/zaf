#include <zaf/rx/internal/subject/replay_subject_core.h>

namespace zaf::rx::internal {

ReplaySubjectCore::ReplaySubjectCore(std::optional<std::size_t> replay_size) noexcept :
    replay_size_(replay_size) {

}


std::shared_ptr<SubscriptionCore> ReplaySubjectCore::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    //This instance may be destroyed during the emissions.
    //Keep it alive here.
    auto shared_this = shared_from_this();

    for (const auto& each_value : replay_values_) {
        observer->OnNext(each_value);
    }

    auto exception = std::get_if<std::exception_ptr>(&termination_);
    if (exception) {
        observer->OnError(*exception);
    }
    else if (std::holds_alternative<None>(termination_)) {
        observer->OnCompleted();
    }

    return __super::Subscribe(observer);
}


void ReplaySubjectCore::OnNext(const std::any& value) {

    if (IsTerminated()) {
        return;
    }

    if (replay_size_ == 0) {
        return;
    }

    if (replay_values_.size() == replay_size_) {
        replay_values_.pop_front();
    }

    replay_values_.push_back(value);

    __super::OnNext(value);
}


void ReplaySubjectCore::OnError(const std::exception_ptr& error) {

    if (IsTerminated()) {
        return;
    }

    termination_ = error;

    __super::OnError(error);
}


void ReplaySubjectCore::OnCompleted() {

    if (IsTerminated()) {
        return;
    }

    termination_ = None{};

    __super::OnCompleted();
}


bool ReplaySubjectCore::IsTerminated() const noexcept {
    return !std::holds_alternative<std::monostate>(termination_);
}

}