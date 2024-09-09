#pragma once

#include <zaf/graphic/d2d/renderer.h>

namespace zaf::d2d {

class WindowRenderer : public Renderer {
public:
    WindowRenderer() = default;

    explicit WindowRenderer(COMPtr<ID2D1HwndRenderTarget> ptr);

    void Resize(const Size& size);

    const COMPtr<ID2D1HwndRenderTarget>& Ptr() const noexcept {
        return ptr_;
    }

private:
    COMPtr<ID2D1HwndRenderTarget> ptr_;
};

}