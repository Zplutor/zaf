#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>

namespace zaf::internal {

/**
A core class implements subscription mechanism.
*/
class SubscriptionCore : NonCopyable {
public:
    using FinishNotification = std::function<void(SubscriptionCore*, int)>;

public:
    SubscriptionCore(std::shared_ptr<InnerObserver> observer);
    virtual ~SubscriptionCore() = default;

    void DeliverOnNext(const std::any& any);
    void DeliverOnError(const Error& error);
    void DeliverOnCompleted();

    void FinishSubscription();

    void Unsubscribe();

protected:
    virtual void OnUnsubscribe() = 0;

private:
    friend class InnerSubscriptionHolder;

    std::optional<int> RegisterFinishNotification(FinishNotification callback);
    void UnregisterFinishNotification(int id);

private:
    bool SetFinish();
    void NotifyFinish();

private:
    std::shared_ptr<InnerObserver> observer_;

    bool is_finished_{};
    int id_seed_{};
    std::map<int, FinishNotification> finish_notifications_;
    std::mutex lock_;
};

}