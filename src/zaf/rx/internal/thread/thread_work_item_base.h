#pragma once

#include <zaf/base/closure.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx::internal {

class ThreadWorkItemBase : public Disposable {
public:
    explicit ThreadWorkItemBase(Closure work) : work_(std::move(work)) {

    }

    void RunWork() {
        if (!MarkDone()) {
            return;
        }
        work_();
        Dispose();
    }

    bool IsDisposed() const noexcept override final {
        return (state_flags_ & StateFlagDisposed) != 0;
    }

protected:
    bool EnsureDisposed() noexcept override final {
        if (!MarkDisposed()) {
            return false;
        }
        work_ = nullptr;
        OnDispose();
        return true;
    }

    virtual void OnDispose() noexcept = 0;

private:
    static constexpr int StateFlagDone = 1 << 0;
    static constexpr int StateFlagDisposed = 1 << 1;

private:
    bool MarkDone() noexcept {
        int old_flags = state_flags_.fetch_or(StateFlagDone);
        return (old_flags & StateFlagDone) == 0;
    }

    bool MarkDisposed() noexcept {
        int new_flags = StateFlagDone | StateFlagDisposed;
        int old_flags = state_flags_.fetch_or(new_flags);
        return (old_flags & StateFlagDisposed) == 0;
    }

private:
    std::atomic<int> state_flags_{};
    Closure work_;
};

}