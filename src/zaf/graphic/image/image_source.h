#pragma once

#include <wincodec.h>
#include <cstdint>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class ImagePalette;

class ImageSource : public ComObject<IWICBitmapSource> {
public:
    ImageSource() { }
    explicit ImageSource(IWICBitmapSource* handle) : ComObject(handle) { }

    const Size GetSize(std::error_code& error_code) const;

    const Size GetSize() const {
        std::error_code error_code;
        auto result = GetSize(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const std::pair<double, double> GetResolution(std::error_code& error_code) const;

    const std::pair<double, double> GetResolution() const {
        std::error_code error_code;
        auto result = GetResolution(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    void CopyPixels(
        const Rect& rect,
        std::uint32_t stride, 
        std::size_t buffer_size, 
        std::uint8_t* buffer, 
        std::error_code& error_code) const;

    void CopyPixels(
        const Rect& rect,
        std::uint32_t stride,
        std::size_t buffer_size,
        std::uint8_t* buffer) const {

        std::error_code error_code;
        CopyPixels(rect, stride, buffer_size, buffer);
        ZAF_CHECK_ERROR(error_code);
    }

    void CopyPalette(ImagePalette& palette, std::error_code& error_code) const;

    void CopyPalette(ImagePalette& palette) const {
        std::error_code error_code;
        CopyPalette(palette, error_code);
        ZAF_CHECK_ERROR(error_code);
    }
};

}