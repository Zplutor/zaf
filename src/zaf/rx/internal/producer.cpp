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

    //Finish the produce before disposal.
    FinishProduce();

    //Do disposal.
    if (MarkDisposed()) {
        OnDispose();
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


bool Producer::MarkDisposed() {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_disposed_) {
        return false;
    }
    is_disposed_ = true;
    return true;
}

}