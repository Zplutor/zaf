#include <zaf/rx/subscription_bag.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx {

SubscriptionBag::SubscriptionBag() noexcept {

}


SubscriptionBag::~SubscriptionBag() {
    Clear();
}


void SubscriptionBag::Add(const Subscription& subscription) {

    const auto& core = subscription.Core();
    if (!core) {
        return;
    }

    if (core->IsDisposed()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(lock_);
        if (items_.contains(core)) {
            return;
        }
        items_.insert(core);
    }

    std::weak_ptr<Disposable> weak_core = core;
    core->AddDisposedCallback([this, weak_core]() {

        auto core = weak_core.lock();
        if (!core) {
            return;
        }

        std::lock_guard<std::mutex> lock(lock_);
        items_.erase(core);
    });
}


void SubscriptionBag::operator+=(const Subscription& subscription) {
    Add(subscription);
}


void SubscriptionBag::Clear() noexcept {

    std::unordered_set<std::shared_ptr<Disposable>> disposing_items;
    {
        std::lock_guard<std::mutex> lock(lock_);
        disposing_items = std::move(items_);
    }

    for (const auto& item : disposing_items) {
        item->Dispose();
    }
}


std::size_t SubscriptionBag::Count() const noexcept {
    std::lock_guard<std::mutex> lock(lock_);
    return items_.size();
}

}