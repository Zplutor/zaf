#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class WindowRenderer : public Renderer {
public:
    WindowRenderer() = default;
    explicit WindowRenderer(COMPtr<ID2D1HwndRenderTarget> inner) :
        Renderer(inner),
        inner_(std::move(inner)) { }

    void Resize(const Size& size) {
        HRESULT result = Inner()->Resize(size.ToD2D1SIZEU());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    const COMPtr<ID2D1HwndRenderTarget>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1HwndRenderTarget> inner_;
};

}