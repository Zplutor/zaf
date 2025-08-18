#include <zaf/rx/disposable.h>

namespace zaf::rx {
namespace {

class EmptyDisposable : public Disposable {
public:
    EmptyDisposable() {
        // The default constructor of Disposable initializes disposed_callbacks_ to an empty 
        // vector. We need to call Dispose() immediately to reset it to std::nullopt to indicate 
        // that this instance is disposed.
        this->Dispose();
        is_dispose_called_ = true;
    }

    bool IsDisposed() const noexcept override {
        return true;
    }

protected:
    bool EnsureDisposed() noexcept override {
        if (is_dispose_called_) {
            return false;
        }
        return true;
    }

private:
    bool is_dispose_called_{ false };
};

} // namespace

const std::shared_ptr<Disposable>& Disposable::Empty() {    
    static std::shared_ptr<Disposable> instance = std::make_shared<EmptyDisposable>();
    return instance;
}


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