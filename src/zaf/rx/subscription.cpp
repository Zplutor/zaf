#include <zaf/rx/subscription.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

Subscription::Subscription(std::shared_ptr<Disposable> core) noexcept :
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