#include <zaf/rx/disposable.h>

namespace zaf::rx {

Disposable::Disposable() noexcept {
    disposed_callbacks_.emplace();
}


void Disposable::Dispose() noexcept {

    if (EnsureDisposed()) {
        InvokeDisposedCallbacks();
    }
}


void Disposable::AddDisposedCallback(Closure callback) {

    {
        std::lock_guard<std::mutex> lock(disposed_callbacks_mutex_);
        if (disposed_callbacks_) {
            disposed_callbacks_->push_back(std::move(callback));
        }
    }

    // Current object has been disposed, invoke the callback immediately.
    if (callback) {
        callback();
    }
}


void Disposable::InvokeDisposedCallbacks() noexcept {

    std::vector<Closure> callbacks;
    {
        std::lock_guard<std::mutex> lock(disposed_callbacks_mutex_);
        if (disposed_callbacks_) {
            callbacks = std::move(*disposed_callbacks_);
            disposed_callbacks_.reset();
        }
    }

    for (const auto& callback : callbacks) {
        callback();
    }
}

}