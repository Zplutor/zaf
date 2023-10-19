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
    auto keep_alive = shared_from_this();
    if (observer_) {
        observer_->OnError(error);
    }
    FinishProduce();
}


void Producer::DeliverOnCompleted() {
    //There is a delicate protection here. 
    //During the OnCompleted() notification chain, current producer might be released, causing a 
    //dangling pointer. Exception would occur if execution continue.
    //So we have to retain a shared pointer to keep producer alive during the notification.
    auto keep_alive = shared_from_this();
    if (observer_) {
        observer_->OnCompleted();
    }
    FinishProduce();
}


void Producer::Dispose() {

    if (MarkDisposed()) {

        //Finish the produce before disposal.
        FinishProduce();
    
        OnDispose();

        NotifyDispose();

        //Release observer to break potential circular reference.
        observer_.reset();
    }
}


void Producer::FinishProduce() {

    if (MarkFinished()) {
        NotifyFinish();
    }
}


bool Producer::MarkFinished() {
    bool expected{ false };
    return is_finished_.compare_exchange_strong(expected, true);
}


void Producer::NotifyFinish() {

    FinishNotification notification;
    {
        std::scoped_lock<std::mutex> lock(lock_);
        notification = std::move(finish_notification_);
    }

    if (notification) {
        notification();
    }
}


void Producer::RegisterFinishNotification(FinishNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return;
    }

    finish_notification_ = std::move(callback);
}


std::optional<int> Producer::RegisterDisposeNotification(DisposeNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_finished_) {
        return std::nullopt;
    }

    auto id = ++dispose_notification_id_seed_;
    dispose_notifications_[id] = std::move(callback);
    return id;
}


void Producer::UnregisterDisposeNotification(int id) {

    std::scoped_lock<std::mutex> lock(lock_);
    dispose_notifications_.erase(id);
}


bool Producer::MarkDisposed() {
    bool expected{ false };
    return is_disposed_.compare_exchange_strong(expected, true);
}


void Producer::NotifyDispose() {

    std::map<int, DisposeNotification> notifications;
    {
        std::scoped_lock<std::mutex> lock(lock_);
        notifications = std::move(dispose_notifications_);
    }

    for (const auto& each_pair : notifications) {
        each_pair.second(this, each_pair.first);
    }
}

}