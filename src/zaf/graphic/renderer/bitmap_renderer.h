#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class BitmapRenderer : public Renderer {
public:
    BitmapRenderer() = default;
    BitmapRenderer(COMPtr<ID2D1BitmapRenderTarget> ptr) :
        Renderer(ptr),
        inner_(std::move(ptr)) { }

    RenderBitmap GetBitmap() const;

    const COMPtr<ID2D1BitmapRenderTarget>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1BitmapRenderTarget> inner_;
};

}