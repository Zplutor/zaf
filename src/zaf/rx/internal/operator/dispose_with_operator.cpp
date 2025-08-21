#include <zaf/rx/internal/operator/dispose_with_operator.h>
#include <optional>

namespace zaf::rx::internal {

DisposeWithOperator::DisposeWithOperator(std::shared_ptr<ObservableCore> source) noexcept :
    source_(std::move(source)),
    composite_disposable_(std::make_shared<CompositeDisposable>()) {

}


DisposeWithOperator::~DisposeWithOperator() {
    composite_disposable_->DisposeIfEmpty();
}


std::shared_ptr<Disposable> DisposeWithOperator::Subscribe(ObserverShim&& observer) {

    if (composite_disposable_->IsDisposed()) {
        return Disposable::Empty();
    }

    auto sub = source_->Subscribe(std::move(observer));
    composite_disposable_->Add(sub);
    return sub;
}


DisposeWithOperator::CompositeDisposable::CompositeDisposable() {

}


DisposeWithOperator::CompositeDisposable::~CompositeDisposable() {
    EnsureDisposed();
}


void DisposeWithOperator::CompositeDisposable::Add(const std::shared_ptr<Disposable>& disposable) {

    if (disposable->IsDisposed()) {
        return;
    }

    std::optional<std::size_t> id;
    if (!is_disposed_) {

        std::lock_guard<std::mutex> lock(mutex_);
        // Double check.
        if (!is_disposed_) {

            id = disposable_id_seed_++;
            disposables_[*id] = disposable;
        }
    }

    if (!id) {
        disposable->Dispose();
        return;
    }

    try {
        disposable->AddDisposedCallback([this, id]() {
            bool is_empty{};
            {
                std::lock_guard<std::mutex> lock(mutex_);
                disposables_.erase(*id);
                is_empty = disposables_.empty();
            }
            if (is_empty) {
                Dispose();
            }
        });
    }
    catch (...) {
        std::lock_guard<std::mutex> lock(mutex_);
        disposables_.erase(*id);
        throw;
    }
}


void DisposeWithOperator::CompositeDisposable::DisposeIfEmpty() noexcept {

    bool is_empty{};
    {
        std::lock_guard<std::mutex> lock(mutex_);
        is_empty = disposables_.empty();
    }

    if (is_empty) {
        Dispose();
    }
}


bool DisposeWithOperator::CompositeDisposable::EnsureDisposed() noexcept {

    bool expected{ false };
    if (!is_disposed_.compare_exchange_strong(expected, true)) {
        return false;
    }

    std::unordered_map<std::size_t, std::weak_ptr<Disposable>> disposables;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        disposables = std::move(disposables_);
    }

    for (auto& [id, weak_disposable] : disposables) {
        if (auto disposable = weak_disposable.lock()) {
            disposable->Dispose();
        }
    }
    return true;
}

}
