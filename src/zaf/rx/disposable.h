#pragma once

#include <mutex>
#include <optional>
#include <vector>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Disposable : NonCopyableNonMovable {
public:
    static const std::shared_ptr<Disposable>& Empty();

public:
    Disposable() noexcept;
    virtual ~Disposable() = default;

    virtual bool IsDisposed() const noexcept = 0;

    void Dispose() noexcept;

    void AddDisposedCallback(Closure callback);

protected:
    virtual bool EnsureDisposed() noexcept = 0;

private:
    void InvokeDisposedCallbacks() noexcept;

private:
    std::mutex disposed_callbacks_mutex_;
    // Will be set to std::nullopt after disposed.
    std::optional<std::vector<Closure>> disposed_callbacks_;
};

}