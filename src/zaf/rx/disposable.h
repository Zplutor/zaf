#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Disposable : NonCopyableNonMovable {
public:
    Disposable() = default;
    virtual ~Disposable() = default;

    virtual void Dispose() noexcept = 0;
    virtual bool IsDisposed() const noexcept = 0;
};

}