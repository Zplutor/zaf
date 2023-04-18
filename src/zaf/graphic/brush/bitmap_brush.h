#pragma once

#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/interpolation_mode.h>
#include <zaf/graphic/render_bitmap.h>

namespace zaf {

class BitmapBrush : public Brush {
public:
    BitmapBrush() { }
    explicit BitmapBrush(ID2D1BitmapBrush* handle) : Brush(handle) { }

    const RenderBitmap GetBitmap() const {
        ID2D1Bitmap* handle = nullptr;
        Inner()->GetBitmap(&handle);
        return RenderBitmap(handle);
    }

    void SetBitmap(const RenderBitmap& bitmap) {
        Inner()->SetBitmap(bitmap.Inner());
    }

    InterpolationMode GetInterpolationMode() const {
        return static_cast<InterpolationMode>(Inner()->GetInterpolationMode());
    }

    void SetInterpolationMode(InterpolationMode mode) {
        Inner()->SetInterpolationMode(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(mode));
    }

    ID2D1BitmapBrush* Inner() const {
        return static_cast<ID2D1BitmapBrush*>(__super::Inner());
    }
};

}