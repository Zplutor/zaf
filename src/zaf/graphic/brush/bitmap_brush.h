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
        GetHandle()->GetBitmap(&handle);
        return RenderBitmap(handle);
    }

    void SetBitmap(const RenderBitmap& bitmap) {
        GetHandle()->SetBitmap(bitmap.GetHandle());
    }

    InterpolationMode GetInterpolationMode() const {
        return static_cast<InterpolationMode>(GetHandle()->GetInterpolationMode());
    }

    void SetInterpolationMode(InterpolationMode mode) {
        GetHandle()->SetInterpolationMode(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(mode));
    }

    ID2D1BitmapBrush* GetHandle() const {
        return static_cast<ID2D1BitmapBrush*>(__super::GetHandle());
    }
};

}