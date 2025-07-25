#include <zaf/rx/internal/operator/ref_count_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

RefCountOperator::RefCountOperator(std::shared_ptr<ConnectableObservableCore> source) :
    source_(std::move(source)),
    shared_lock_(std::make_shared<std::mutex>()) {

}


std::shared_ptr<SubscriptionCore> RefCountOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    bool should_connect{};
    std::shared_ptr<Connection> connection;
    {
        std::lock_guard<std::mutex> lock(*shared_lock_);
        if (!current_connection_) {
            current_connection_ = std::make_shared<Connection>();
            current_connection_->shared_lock = shared_lock_;
            current_connection_->owner = As<RefCountOperator>(shared_from_this());
            should_connect = true;
        }
        current_connection_->ref_count++;
        connection = current_connection_;
    }

    auto sub = source_->Subscribe(observer);
    if (!sub) {
        // If the subscription is null, it means the observer is already unsubscribed.
        // In this case, we decrease the reference count and return.
        DecreaseRef(connection);
        return nullptr;
    }

    auto notification_id = sub->RegisterUnsubscribeNotification(std::bind([connection]() {
        DecreaseRef(connection);
    }));

    // The subscription may terminate immediately, in which case we decrease the reference count.
    if (!notification_id) {
        DecreaseRef(connection);
        return sub;
    }

    if (should_connect) {
        auto subscription = source_->Connect();
        InstallSubscription(connection, std::move(subscription));
    }
    return sub;
}


void RefCountOperator::InstallSubscription(
    const std::shared_ptr<Connection>& connection, 
    std::shared_ptr<SubscriptionCore> subscription) {

    // The subscription of the connection may be null if the underlying observable is cold.
    if (!subscription) {
        return;
    }

    std::lock_guard<std::mutex> lock(*connection->shared_lock);

    // If the connection is already disconnected, do nothing.
    if (connection->ref_count == 0) {
        return;
    }

    connection->subscription = std::move(subscription);
}


void RefCountOperator::DecreaseRef(const std::shared_ptr<Connection>& connection) {

    std::shared_ptr<SubscriptionCore> need_unsubscribe_subscription;

    {
        std::lock_guard<std::mutex> lock(*connection->shared_lock);
        if (connection->ref_count == 0) {
            return;
        }

        connection->ref_count--;

        // The connection is still alive.
        if (connection->ref_count > 0) {
            return;
        }

        // The reference count reaches 0, the connection should be disconnected.
        // Reset the connection in the owner first.
        auto owner = connection->owner.lock();
        if (owner) {
            if (owner->current_connection_ == connection) {
                owner->current_connection_.reset();
            }
        }

        need_unsubscribe_subscription = std::move(connection->subscription);
    }

    if (need_unsubscribe_subscription) {
        need_unsubscribe_subscription->Unsubscribe();
    }
}

}