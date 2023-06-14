#pragma once

#include <zaf/base/com_ptr.h>

namespace zaf {

template<typename T>
class COMObject {
public:
    COMObject() noexcept = default;
    explicit COMObject(COMPtr<T> inner) noexcept : inner_(std::move(inner)) { }
    virtual ~COMObject() = default;

    const COMPtr<T>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<T> inner_;
};

}