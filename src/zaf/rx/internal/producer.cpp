#include <zaf/rx/internal/producer.h>
#include <vector>

namespace zaf::rx::internal {

Producer::Producer(ObserverShim&& observer) noexcept : observer_(std::move(observer)) {

}


bool Producer::EmitOnNext(const std::any& any) {

    if (IsTerminated()) {
        return false;
    }

    observer_.OnNext(any);
    return true;
}


bool Producer::EmitOnError(const std::exception_ptr& error) {

    if (!MarkTerminated()) {
        return false;
    }

    auto keep_alive = shared_from_this();
    observer_.OnError(error);

    Dispose();
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
    observer_.OnCompleted();

    Dispose();
    return true;
}


void Producer::Dispose() noexcept {

    if (!MarkDisposed()) {
        return;
    }

    OnDispose();
    SendDisposeNotifications();

    //Free the observer to break potential circular reference.
    observer_ = {};
}


bool Producer::MarkTerminated() noexcept {
    int old_flags = state_flags_.fetch_or(StateFlagTerminated);
    return (old_flags & StateFlagTerminated) == 0;
}


bool Producer::IsTerminated() const noexcept {
    return (state_flags_ & StateFlagTerminated) != 0;
}


bool Producer::MarkDisposed() noexcept {
    int new_flags = StateFlagTerminated | StateFlagDisposed;
    int old_flags = state_flags_.fetch_or(new_flags);
    return (old_flags & StateFlagDisposed) == 0;
}


bool Producer::IsDisposed() const noexcept {
    return (state_flags_ & StateFlagDisposed) != 0;
}


std::optional<DisposeNotificationID> Producer::RegisterDisposeNotification(
    DisposeNotification callback) {

    std::lock_guard<std::mutex> lock(dispose_notification_lock_);
    if (IsDisposed()) {
        return std::nullopt;
    }

    auto id = std::make_pair(
        reinterpret_cast<std::uintptr_t>(this), 
        ++dispose_notification_id_seed_);

    dispose_notifications_[id] = std::move(callback);
    return id;
}


void Producer::UnregisterDisposeNotification(DisposeNotificationID id) {

    std::lock_guard<std::mutex> lock(dispose_notification_lock_);
    dispose_notifications_.erase(id);
}


void Producer::SendDisposeNotifications() noexcept {

    std::map<DisposeNotificationID, DisposeNotification> notifications;
    {
        std::lock_guard<std::mutex> lock(dispose_notification_lock_);
        notifications = std::move(dispose_notifications_);
    }

    for (const auto& each_pair : notifications) {
        each_pair.second(each_pair.first);
    }
}

}