#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx {

Subscription::Subscription(std::shared_ptr<rx::internal::SubscriptionCore> core) noexcept :
    core_(std::move(core)) {

}


Subscription::~Subscription() {

}


void Subscription::Dispose() noexcept {
    if (core_) {
        core_->Dispose();
    }
}


bool Subscription::IsDisposed() const noexcept {
    if (core_) {
        return core_->IsDisposed();
    }
    return true;
}

}