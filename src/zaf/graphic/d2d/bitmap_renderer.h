#pragma once

#include <zaf/graphic/d2d/renderer.h>

namespace zaf::d2d {

class BitmapRenderer : public Renderer {
public:
    BitmapRenderer() = default;

    explicit BitmapRenderer(COMPtr<ID2D1BitmapRenderTarget> ptr);

    RenderBitmap GetBitmap() const;

    const COMPtr<ID2D1BitmapRenderTarget>& Ptr() const noexcept {
        return ptr_;
    }

private:
    COMPtr<ID2D1BitmapRenderTarget> ptr_;
};

}