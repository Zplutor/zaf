#include <zaf/rx/dispose_bag.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

DisposeBag::DisposeBag() noexcept {

}


DisposeBag::~DisposeBag() {
    Clear();
}


void DisposeBag::Add(const std::shared_ptr<Disposable>& disposable) {

    if (!disposable) {
        return;
    }

    if (disposable->IsDisposed()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(lock_);
        if (items_.contains(disposable)) {
            return;
        }
        items_.insert(disposable);
    }

    std::weak_ptr<Disposable> weak = disposable;

    try {
        disposable->AddDisposedCallback([this, weak]() {

            auto strong = weak.lock();
            if (!strong) {
                return;
            }

            std::lock_guard<std::mutex> lock(lock_);
            items_.erase(strong);
        });
    }
    catch (...) {
        // Remove the added item if fail to add the disposed callback.
        {
            std::lock_guard<std::mutex> lock(lock_);
            items_.erase(disposable);
        }
        throw;
    }
}


void DisposeBag::operator+=(const std::shared_ptr<Disposable>& disposable) {
    Add(disposable);
}


void DisposeBag::Clear() noexcept {

    std::unordered_set<std::shared_ptr<Disposable>> disposing_items;
    {
        std::lock_guard<std::mutex> lock(lock_);
        disposing_items = std::move(items_);
    }

    for (const auto& item : disposing_items) {
        item->Dispose();
    }
}


std::size_t DisposeBag::Count() const noexcept {
    std::lock_guard<std::mutex> lock(lock_);
    return items_.size();
}

}