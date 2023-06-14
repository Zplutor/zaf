#pragma once

#include <zaf/base/com_ptr.h>

namespace zaf {

template<typename T>
class COMObject {
public:
    COMObject() noexcept = default;
    explicit COMObject(COMPtr<T> inner) noexcept : inner_(std::move(inner)) { }

    const COMPtr<T>& Inner() const noexcept {
        return inner_;
    }

    bool IsValid() const noexcept {
        return inner_.IsValid();
    }

    explicit operator bool() const noexcept {
        return IsValid();
    }

private:
    COMPtr<T> inner_;
};

}