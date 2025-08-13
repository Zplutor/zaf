#pragma once

#include <mutex>
#include <zaf/rx/internal/observable/connectable_observable_core.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

class RefCountOperator : public ObservableCore {
public:
    class Connection {
    public:
        std::shared_ptr<std::mutex> shared_lock;
        std::weak_ptr<RefCountOperator> owner;
        std::shared_ptr<SubscriptionCore> subscription;
        std::size_t ref_count{};
    };

public:
    explicit RefCountOperator(std::shared_ptr<ConnectableObservableCore> source);

    std::shared_ptr<SubscriptionCore> Subscribe(ObserverShim&& observer) override;

    // For unit test.
    bool HasConnection() const noexcept {
        return current_connection_ != nullptr;
    }

private:
    static void InstallSubscription(
        const std::shared_ptr<Connection>& connection,
        std::shared_ptr<SubscriptionCore> subscription);
    static void DecreaseRef(const std::shared_ptr<Connection>& connection);

private:
    std::shared_ptr<ConnectableObservableCore> source_;
    std::shared_ptr<Connection> current_connection_;
    // A lock shared among all connections and the operator.
    std::shared_ptr<std::mutex> shared_lock_;
};

}