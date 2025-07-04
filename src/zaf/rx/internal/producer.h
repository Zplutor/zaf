#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {

/**
A producer is responsible for emitting data sequence.

@details
    There are two states of a producer: terminated and disposed.

    Terminated state indicates that the data sequence has ended. It's set after OnError and 
    OnCompleted.

    Disposed state indicates that any resource held by the producer has been destroyed. A disposed
    state always follows after a terminated state. Or it can be set explicitly by calling 
    Dispose().
*/
class Producer : public std::enable_shared_from_this<Producer>, NonCopyableNonMovable {
public:
    explicit Producer(std::shared_ptr<ObserverCore> observer);
    virtual ~Producer() = default;

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

    bool IsTerminated() const noexcept {
        return is_terminated_;
    }

    /**
    Disposes the producer.

    This method is called by SubscriptionCore to cancel subscription and to do clean up work.
    It's legal to call this method multiple times, only the first call would take effect.
    */
    void Dispose();

protected:
    /**
    Override this method in derived classes to do specific disposal work.

    Any shared pointer should be reset in this method, in order to break potential circular 
    references.
    */
    virtual void OnDispose() = 0;

private:
    friend class InnerSubscriptionSet;

    using DisposeNotification = std::function<void(Producer*, int)>;
    std::optional<int> RegisterDisposeNotification(DisposeNotification callback);
    void UnregisterDisposeNotification(int id);

private:
    void Terminate();
    bool MarkTerminated();

    bool MarkDisposed();
    void NotifyDispose();

private:
    std::shared_ptr<ObserverCore> observer_;

    std::atomic<bool> is_terminated_{};
    std::atomic<bool> is_disposed_{};
    int dispose_notification_id_seed_{};
    std::map<int, DisposeNotification> dispose_notifications_;

    std::mutex lock_;
};

}