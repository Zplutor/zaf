#pragma once

#include <zaf/base/closure.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx::internal {

class DelayedWorkItemBase : public Disposable {
public:
    explicit DelayedWorkItemBase(Closure work) : work_(std::move(work)) {

    }

    Closure TakeWorkIfNotDisposed() {
        if (MarkAsDispose()) {
            return std::move(work_);
        }
        return nullptr;
    }

    void Dispose() noexcept override final {
        if (!MarkAsDispose()) {
            return;
        }
        work_ = nullptr;
        OnDispose();
    }

    bool IsDisposed() const noexcept override final {
        return is_disposed_;
    }

protected:
    virtual void OnDispose() noexcept = 0;

private:
    bool MarkAsDispose() noexcept {
        bool expected{ false };
        return is_disposed_.compare_exchange_strong(expected, true);
    }

private:
    std::atomic<bool> is_disposed_{ false };
    Closure work_;
};

}