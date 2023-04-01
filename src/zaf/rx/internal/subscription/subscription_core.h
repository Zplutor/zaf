#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/rx/internal/inner_observer.h>

namespace zaf::internal {

class SubscriptionCore {
public:
    using FinishNotification = std::function<void(SubscriptionCore*, int)>;

public:
    SubscriptionCore(std::shared_ptr<InnerObserver> observer);
    virtual ~SubscriptionCore() = default;

    SubscriptionCore(const SubscriptionCore&) = delete;
    SubscriptionCore& operator=(const SubscriptionCore&) = delete;

    void DeliverOnNext(const std::any& any);
    void DeliverOnError(const Error& error);
    void DeliverOnCompleted();

    void FinishSubscription();

    void Unsubscribe();

protected:
    virtual void OnUnsubscribe() = 0;

private:
    friend class InnerSubscriptionSet;

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