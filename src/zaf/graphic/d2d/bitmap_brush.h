#pragma once

#include <zaf/graphic/d2d/brush.h>
#include <zaf/graphic/d2d/interpolation_mode.h>
#include <zaf/graphic/d2d/render_bitmap.h>

namespace zaf::d2d {

class BitmapBrush : public Brush {
public:
    BitmapBrush() = default;

    explicit BitmapBrush(COMPtr<ID2D1BitmapBrush> ptr) : 
        Brush(ptr),
        ptr_(std::move(ptr)) { }

    const RenderBitmap GetBitmap() const {
        COMPtr<ID2D1Bitmap> inner;
        Ptr()->GetBitmap(inner.Reset());
        return RenderBitmap(inner);
    }

    void SetBitmap(const RenderBitmap& bitmap) {
        Ptr()->SetBitmap(bitmap.Ptr().Inner());
    }

    InterpolationMode GetInterpolationMode() const {
        return static_cast<InterpolationMode>(Ptr()->GetInterpolationMode());
    }

    void SetInterpolationMode(InterpolationMode mode) {
        Ptr()->SetInterpolationMode(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(mode));
    }

    const COMPtr<ID2D1BitmapBrush>& Ptr() const noexcept {
        return ptr_;
    }

private:
    COMPtr<ID2D1BitmapBrush> ptr_;
};

}