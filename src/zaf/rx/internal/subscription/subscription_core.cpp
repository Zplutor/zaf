#include <zaf/rx/internal/subscription/subscription_core.h>
#include <vector>

namespace zaf::internal {

SubscriptionCore::SubscriptionCore(std::shared_ptr<ObserverImplementation> observer) :
    observer_(std::move(observer)) {

}


void SubscriptionCore::DeliverOnNext(const std::any& any) {
    if (observer_) {
        observer_->OnNext(any);
    }
}


void SubscriptionCore::DeliverOnError(const Error& error) {
    if (observer_) {
        observer_->OnError(error);
    }
    FinishSubscription();
}


void SubscriptionCore::DeliverOnCompleted() {
    if (observer_) {
        observer_->OnCompleted();
    }
    FinishSubscription();
}


void SubscriptionCore::FinishSubscription() {

    if (SetFinish()) {
        NotifyFinish();
    }
}


void SubscriptionCore::Unsubscribe() {

    if (SetFinish()) {
        OnUnsubscribe();
        NotifyFinish();
    }
}


bool SubscriptionCore::SetFinish() {
    
    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return false;
    }
    is_finished_ = true;
    return true;
}


std::optional<int> SubscriptionCore::RegisterFinishNotification(FinishNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return std::nullopt;
    }

    auto id = ++id_seed_;
    finish_notifications_[id] = std::move(callback);
    return id;
}


void SubscriptionCore::UnregisterFinishNotification(int id) {

    std::scoped_lock<std::mutex> lock(lock_);
    finish_notifications_.erase(id);
}


void SubscriptionCore::NotifyFinish() {

    std::map<int, FinishNotification> notifications;
    {
        std::scoped_lock<std::mutex> lock(lock_);
        notifications = std::move(finish_notifications_);
    }

    for (const auto& each_pair : notifications) {
        each_pair.second(this, each_pair.first);
    }
}

}