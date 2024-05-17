#pragma once

#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/interpolation_mode.h>
#include <zaf/graphic/render_bitmap.h>

namespace zaf {

class BitmapBrush : public Brush {
public:
    BitmapBrush() = default;
    explicit BitmapBrush(COMPtr<ID2D1BitmapBrush> inner) : 
        Brush(inner),
        inner_(std::move(inner)) { }

    const RenderBitmap GetBitmap() const {
        COMPtr<ID2D1Bitmap> inner;
        Inner()->GetBitmap(inner.Reset());
        return RenderBitmap(inner);
    }

    void SetBitmap(const RenderBitmap& bitmap) {
        Inner()->SetBitmap(bitmap.Ptr().Inner());
    }

    InterpolationMode GetInterpolationMode() const {
        return static_cast<InterpolationMode>(Inner()->GetInterpolationMode());
    }

    void SetInterpolationMode(InterpolationMode mode) {
        Inner()->SetInterpolationMode(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(mode));
    }

    const COMPtr<ID2D1BitmapBrush>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1BitmapBrush> inner_;
};

}