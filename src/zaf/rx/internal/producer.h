#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/unsubscribe_notification.h>

namespace zaf::rx::internal {

/**
A producer is responsible for emitting data sequence.

@details
    There are two states of a producer: subscribed and unsubscribed. Subscribed state indicates 
    that the producer is actively emitting values. While unsubscribed state indicates that the 
    producer is no longer emitting values.

    A producer transitions from subscribed state to unsubscribed state either when Unsubscribe() is
    called, or when EmitOnError() or EmitOnCompleted() is called.
*/
class Producer : public std::enable_shared_from_this<Producer>, NonCopyableNonMovable {
public:
    explicit Producer(std::shared_ptr<ObserverCore> observer) noexcept;

    /**
    Emits a data item to the observer.
    */
    void EmitOnNext(const std::any& any);

    /**
    Emits an error to the observer.
    */
    void EmitOnError(const std::exception_ptr& error);

    /**
    Emits a completion to the observer.
    */
    void EmitOnCompleted();

    /**
    Unsubscribes the producer.

    @details
        It's legal to call this method multiple times, only the first call would take effect.
    */
    void Unsubscribe();
    bool IsUnsubscribed() const noexcept;

    std::optional<UnsubscribeNotificationID> RegisterUnsubscribeNotification(
        UnsubscribeNotification callback);

    void UnregisterUnsubscribeNotification(UnsubscribeNotificationID id);

protected:
    /**
    Override this method in derived classes to do specific unsubscription work.

    @details
        Any shared pointer should be reset in this method, in order to break potential circular 
        references.
    */
    virtual void OnUnsubscribe() = 0;

private:
    bool IsTerminated() const noexcept;

    /**
    Marks the producer as terminated.

    @return
        True if the producer was successfully marked as terminated, false if it was already
        terminated.
    */
    bool MarkTerminated();

    /**
    Marks the producer as unsubscribed.

    @return
        True if the producer was successfully marked as unsubscribed, false if it was already
        unsubscribed.

        This method will also mark the producer as terminated.
    */
    bool MarkUnsubscribed();
    void NotifyUnsubscribe();

private:
    static constexpr int StateFlagTerminated = 1 << 0;
    static constexpr int StateFlagUnsubscribed = 1 << 1;

private:
    std::shared_ptr<ObserverCore> observer_;
    std::atomic<int> state_flags_{};
    int unsubscribe_notification_id_seed_{};
    std::map<UnsubscribeNotificationID, UnsubscribeNotification> unsubscribe_notifications_;

    std::mutex unsubscribe_notification_lock_;
};

}