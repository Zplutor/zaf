#pragma once

#include <mutex>
#include <unordered_map>
#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class DisposeWithOperator : public ObservableCore {
public:
    /**
    Represents a disposable that disposes multiple disposables when it is disposed.

    @details
        This class is similar to the `DisposeBag` class, but it holds weak pointers to the 
        disposables rather than strong pointers, to avoid retaining them longer than necessary.
    */
    class CompositeDisposable : public Disposable {
    public:
        CompositeDisposable();
        ~CompositeDisposable();

        bool IsDisposed() const noexcept override {
            return is_disposed_;
        }

        void Add(const std::shared_ptr<Disposable>& disposable);
        void DisposeIfEmpty() noexcept;

    protected:
        bool EnsureDisposed() noexcept override;

    private:
        bool MarkDisposed() noexcept;
        void DoDisposal() noexcept;

    private:
        std::atomic<bool> is_disposed_{ false };
        std::mutex mutex_;
        std::size_t disposable_id_seed_{};
        std::unordered_map<std::size_t, std::weak_ptr<Disposable>> disposables_;
    };

public:
    explicit DisposeWithOperator(std::shared_ptr<ObservableCore> source) noexcept;
    ~DisposeWithOperator();

    const std::shared_ptr<CompositeDisposable>& GetCompositeDisposable() const noexcept {
        return composite_disposable_;
    }

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<CompositeDisposable> composite_disposable_;
};

}