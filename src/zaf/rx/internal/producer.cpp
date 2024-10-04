#include <zaf/rx/internal/producer.h>
#include <vector>

namespace zaf::internal {

Producer::Producer(std::shared_ptr<InnerObserver> observer) :
    observer_(std::move(observer)) {

}


void Producer::EmitOnNext(const std::any& any) {
    if (auto observer = observer_) {
        observer->OnNext(any);
    }
}


void Producer::EmitOnError(const std::exception_ptr& error) {

    auto keep_alive = shared_from_this();
    {
        auto observer = observer_;
        if (observer) {
            observer->OnError(error);
        }
    }
    TerminateProduce();
}


void Producer::EmitOnCompleted() {

    //There is a delicate protection here. 
    //During the OnCompleted() notification chain, current producer might be released, causing a 
    //dangling pointer. Exception would occur if execution continue.
    //So we have to retain a shared pointer to keep producer alive during the notification.
    auto keep_alive = shared_from_this();

    {
        //Here is a similar protection as above. observer_ may be reset in Dispose() during the 
        //OnCompleted() call, so we retain a new shared pointer to keep it alive. 
        auto observer = observer_;
        if (observer) {
            observer->OnCompleted();
        }
    }
    
    TerminateProduce();
}


void Producer::Dispose() {

    if (MarkDisposed()) {

        //Terminate the produce before disposal.
        TerminateProduce();
    
        OnDispose();

        NotifyDispose();

        //Release observer to break potential circular reference.
        observer_.reset();
    }
}


void Producer::TerminateProduce() {

    if (MarkTerminated()) {
        NotifyTerminate();
    }
}


bool Producer::MarkTerminated() {
    bool expected{ false };
    return is_terminated_.compare_exchange_strong(expected, true);
}


void Producer::NotifyTerminate() {

    TerminateNotification notification;
    {
        std::scoped_lock<std::mutex> lock(lock_);
        notification = std::move(terminate_notification_);
    }

    if (notification) {
        notification();
    }
}


void Producer::RegisterTerminateNotification(TerminateNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_terminated_) {
        return;
    }

    terminate_notification_ = std::move(callback);
}


std::optional<int> Producer::RegisterDisposeNotification(DisposeNotification callback) {

    std::scoped_lock<std::mutex> lock(lock_);
    if (is_terminated_) {
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