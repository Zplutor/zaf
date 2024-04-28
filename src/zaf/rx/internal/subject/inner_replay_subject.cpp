#include <zaf/rx/internal/subject/inner_replay_subject.h>

namespace zaf::internal {

std::shared_ptr<InnerSubscription> InnerReplaySubject::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    for (const auto& each_item : replay_items_) {

        std::visit([&observer](const auto& value) {
        
            using ItemType = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<ItemType, std::any>) {
                observer->OnNext(value);
            }
            else if constexpr (std::is_same_v<ItemType, std::exception_ptr>) {
                observer->OnError(value);
            }
            else if constexpr (std::is_same_v<ItemType, None>) {
                observer->OnCompleted();
            }
        }, 
        each_item);
    }

    return __super::Subscribe(observer);
}


void InnerReplaySubject::OnNext(const std::any& value) {

    replay_items_.emplace_back(value);

    __super::OnNext(value);
}


void InnerReplaySubject::OnError(const std::exception_ptr& error) {

    replay_items_.emplace_back(error);

    __super::OnError(error);
}


void InnerReplaySubject::OnCompleted() {

    replay_items_.emplace_back(None{});

    __super::OnCompleted();
}

}