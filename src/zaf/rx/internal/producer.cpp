#include <zaf/rx/internal/producer.h>
#include <vector>

namespace zaf::internal {

Producer::Producer(std::shared_ptr<InnerObserver> observer) :
    observer_(std::move(observer)) {

}


void Producer::DeliverOnNext(const std::any& any) {
    if (observer_) {
        observer_->OnNext(any);
    }
}


void Producer::DeliverOnError(const Error& error) {
    if (observer_) {
        observer_->OnError(error);
    }
    FinishSubscription();
}


void Producer::DeliverOnCompleted() {
    if (observer_) {
        observer_->OnCompleted();
    }
    FinishSubscription();
}


void Producer::FinishSubscription() {

    if (SetFinish()) {
        NotifyFinish();
    }
}


void Producer::Unsubscribe() {

    if (SetFinish()) {
        OnUnsubscribe();
        NotifyFinish();
    }
}


bool Producer::SetFinish() {
    
    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return false;
    }
    is_finished_ = true;
    return true;
}


std::optional<int> Producer::RegisterFinishNotification(FinishNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return std::nullopt;
    }

    auto id = ++id_seed_;
    finish_notifications_[id] = std::move(callback);
    return id;
}


void Producer::UnregisterFinishNotification(int id) {

    std::scoped_lock<std::mutex> lock(lock_);
    finish_notifications_.erase(id);
}


void Producer::NotifyFinish() {

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