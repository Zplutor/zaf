#include <zaf/rx/internal/producer.h>
#include <vector>

namespace zaf::rx::internal {

Producer::Producer(std::shared_ptr<ObserverCore> observer) noexcept : 
    observer_(std::move(observer)) {

}


bool Producer::EmitOnNext(const std::any& any) {

    if (IsTerminated()) {
        return false;
    }

    if (auto observer = observer_) {
        observer->OnNext(any);
    }
    return true;
}


bool Producer::EmitOnError(const std::exception_ptr& error) {

    if (!MarkTerminated()) {
        return false;
    }

    auto keep_alive = shared_from_this();
    {
        if (auto observer = observer_) {
            observer->OnError(error);
        }
    }
    Unsubscribe();
    return true;
}


bool Producer::EmitOnCompleted() {

    if (!MarkTerminated()) {
        return false;
    }

    //There is a delicate protection here. 
    //During the OnCompleted() notification chain, current producer might be released, causing a 
    //dangling pointer. Exception would occur if execution continue.
    //So we have to retain a shared pointer to keep producer alive during the notification.
    auto keep_alive = shared_from_this();
    {
        //Here is a similar protection as above. observer_ may be reset in Dispose() during the 
        //OnCompleted() call, so we retain a new shared pointer to keep it alive. 
        if (auto observer = observer_) {
            observer->OnCompleted();
        }
    }

    Unsubscribe();
    return true;
}


void Producer::Unsubscribe() noexcept {

    if (!MarkUnsubscribed()) {
        return;
    }

    OnUnsubscribe();
    SendUnsubscribeNotifications();

    //Release observer to break potential circular reference.
    observer_.reset();
}


bool Producer::MarkTerminated() noexcept {
    int old_flags = state_flags_.fetch_or(StateFlagTerminated);
    return (old_flags & StateFlagTerminated) == 0;
}


bool Producer::IsTerminated() const noexcept {
    return (state_flags_ & StateFlagTerminated) != 0;
}


bool Producer::MarkUnsubscribed() noexcept {
    int new_flags = StateFlagTerminated | StateFlagUnsubscribed;
    int old_flags = state_flags_.fetch_or(new_flags);
    return (old_flags & StateFlagUnsubscribed) == 0;
}


bool Producer::IsUnsubscribed() const noexcept {
    return (state_flags_ & StateFlagUnsubscribed) != 0;
}


std::optional<UnsubscribeNotificationID> Producer::RegisterUnsubscribeNotification(
    UnsubscribeNotification callback) {

    std::scoped_lock<std::mutex> lock(unsubscribe_notification_lock_);
    if (IsUnsubscribed()) {
        return std::nullopt;
    }

    auto id = std::make_pair(
        reinterpret_cast<std::uintptr_t>(this), 
        ++unsubscribe_notification_id_seed_);

    unsubscribe_notifications_[id] = std::move(callback);
    return id;
}


void Producer::UnregisterUnsubscribeNotification(UnsubscribeNotificationID id) {

    std::scoped_lock<std::mutex> lock(unsubscribe_notification_lock_);
    unsubscribe_notifications_.erase(id);
}


void Producer::SendUnsubscribeNotifications() noexcept {

    std::map<UnsubscribeNotificationID, UnsubscribeNotification> notifications;
    {
        std::scoped_lock<std::mutex> lock(unsubscribe_notification_lock_);
        notifications = std::move(unsubscribe_notifications_);
    }

    for (const auto& each_pair : notifications) {
        each_pair.second(each_pair.first);
    }
}

}