#pragma once

#include <wincodec.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf {

class ImageDecoder : public ComObject<IWICBitmapDecoder> {
public:
    class Frame : public ComObject<IWICBitmapFrameDecode> {
    public:
        Frame() { }
        explicit Frame(IWICBitmapFrameDecode* handle) : ComObject(handle) { }
    };

public:
    ImageDecoder() { }
    explicit ImageDecoder(IWICBitmapDecoder* handle) : ComObject(handle) { }

    std::size_t GetFrameCount(std::error_code& error_code) const;

    std::size_t GetFrameCount() const {
        std::error_code error_code;
        auto result = GetFrameCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const Frame GetFrame(std::size_t index, std::error_code& error_code) const;

    const Frame GetFrame(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFrame(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}