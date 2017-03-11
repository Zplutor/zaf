#pragma once

#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/image/bitmap_properties.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Bitmap : public ComObject<ID2D1Bitmap> {
public:
    Bitmap() { }
    explicit Bitmap(ID2D1Bitmap* handle) : ComObject(handle) { }

    /**
     Get the size, in device-independent pixels (DIPs), of the bitmap.
     */
    const Size GetSize() const {
        return Size::FromD2D1SIZEF(GetHandle()->GetSize());
    }

    /**
     Get the size, in device-dependent units (pixels), of the bitmap.
     */
    const Size GetPixelSize() const {
        return Size::FromD2D1SIZEU(GetHandle()->GetPixelSize());
    }

    /**
     Get dots per inch (DPI) of the bitmap.

     @return
         The first in returned pair is horizontal DPI, and
         the second is vertical DPI.
     */
    const std::pair<float, float> GetDpi() const {
        std::pair<float, float> pair;
        GetHandle()->GetDpi(&pair.first, &pair.second);
        return pair;
    }

    PixelFormat GetPixelFormat() const {
        auto pixel_format = GetHandle()->GetPixelFormat();
        PixelFormat result;
        result.format = pixel_format.format;
        result.alpha_mode = static_cast<PixelFormat::AlphaMode>(pixel_format.alphaMode);
        return result;
    }

    void CopyFromBitmap(const Bitmap& bitmap, std::error_code& error_code) {
        HRESULT com_error = GetHandle()->CopyFromBitmap(nullptr, bitmap.GetHandle(), nullptr);
        error_code = MakeComErrorCode(com_error);
    }

    void CopyFromBitmap(const Bitmap& bitmap) {
        std::error_code error_code;
        CopyFromBitmap(bitmap, error_code);
        ZAF_CHECK_ERROR(error_code);
    }
};

}