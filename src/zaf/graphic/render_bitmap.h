#pragma once

#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/bitmap_properties.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class Renderer;

class RenderBitmap : public ComObject<ID2D1Bitmap> {
public:
    RenderBitmap() { }
    explicit RenderBitmap(ID2D1Bitmap* handle) : ComObject(handle) { }

    /**
     Get the size, in device-independent pixels (DIPs), of the bitmap.
     */
    Size GetSize() const {
        return Size::FromD2D1SIZEF(GetHandle()->GetSize());
    }

    /**
     Get the size, in device-dependent units (pixels), of the bitmap.
     */
    Size GetPixelSize() const {
        return Size::FromD2D1SIZEU(GetHandle()->GetPixelSize());
    }

    /**
     Get dots per inch (DPI) of the bitmap.

     @return
         The first in returned pair is horizontal DPI, and
         the second is vertical DPI.
     */
    std::pair<float, float> GetDpi() const {
        std::pair<float, float> pair;
        GetHandle()->GetDpi(&pair.first, &pair.second);
        return pair;
    }

    PixelProperties GetPixelProperties() const {
        auto d2d_result = GetHandle()->GetPixelFormat();
        PixelProperties result;
        result.format = static_cast<PixelFormat>(d2d_result.format);
        result.alpha_mode = static_cast<PixelProperties::AlphaMode>(d2d_result.alphaMode);
        return result;
    }

    void CopyFromBitmap(const RenderBitmap& bitmap, std::error_code& error_code) {
        HRESULT com_error = GetHandle()->CopyFromBitmap(nullptr, bitmap.GetHandle(), nullptr);
        error_code = MakeComErrorCode(com_error);
    }

    void CopyFromBitmap(const RenderBitmap& bitmap) {
        std::error_code error_code;
        CopyFromBitmap(bitmap, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void CopyFromRenderer(const Renderer& renderer, const Rect& renderer_rect, const Point& to_position, std::error_code& error_code);
    void CopyFromRenderer(const Renderer& renderer, const Rect& renderer_rect, const Point& to_position) {
        std::error_code error_code;
        CopyFromRenderer(renderer, renderer_rect, to_position, error_code);
        ZAF_CHECK_ERROR(error_code);
    }
};

}