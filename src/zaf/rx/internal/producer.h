#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>

namespace zaf::internal {

/**
A producer is responsible for emmiting data sequence.

There are two states of a producer: finished and disposed.

Finished state indicates that the data sequence has ended. It's often set after OnError and 
OnCompleted, or after calling Dispose() explicitly.

Disposed state indicates that any resource held by the producer has been destroyed. A disposed
state always follows after a finished state. It can only be set after calling Dispose().
*/
class Producer : NonCopyableNonMovable {
public:
    explicit Producer(std::shared_ptr<InnerObserver> observer);
    virtual ~Producer() = default;

    void DeliverOnNext(const std::any& any);
    void DeliverOnError(const Error& error);
    void DeliverOnCompleted();

    /**
    Disposes the producer.

    This method is called by InnerSubscription to cancel subscription and to do clean up work.
    It's legal to call this method multiple times, only the first call would take effect.
    */
    void Dispose();

protected:
    /**
    Overrided by derived classes to do specific disposal work.
    */
    virtual void OnDispose() = 0;

private:
    friend class InnerSubscription;
    friend class InnerSubscriptionSet;

    using FinishNotification = std::function<void(Producer*, int)>;
    std::optional<int> RegisterFinishNotification(FinishNotification callback);
    void UnregisterFinishNotification(int id);

private:
    void FinishProduce();
    bool MarkFinished();
    void NotifyFinish();

    bool MarkDisposed();

private:
    std::shared_ptr<InnerObserver> observer_;

    bool is_finished_{};
    bool is_disposed_{};

    int id_seed_{};
    std::map<int, FinishNotification> finish_notifications_;

    std::mutex lock_;
};

}