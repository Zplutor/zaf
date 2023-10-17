#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>

namespace zaf::internal {

class Producer : NonCopyableNonMovable {
public:
    using FinishNotification = std::function<void(Producer*, int)>;

public:
    explicit Producer(std::shared_ptr<InnerObserver> observer);
    virtual ~Producer() = default;

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