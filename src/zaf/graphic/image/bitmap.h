#pragma once

#include <filesystem>
#include <zaf/base/error.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/size.h>

namespace zaf {
namespace wic {
class BitmapDecoder;
class BitmapSource;
}

class Bitmap : public Image {
public:
    static std::shared_ptr<Bitmap> CreateFromFile(
        const std::filesystem::path& file_path,
        std::error_code& error_code);

    static std::shared_ptr<Bitmap> CreateFromFile(const std::filesystem::path& file_path) {
        std::error_code error_code;
        auto result = CreateFromFile(file_path, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    static std::shared_ptr<Bitmap> CreateFromWicBitmap(const wic::BitmapSource& wic_bitmap);

    static std::shared_ptr<Bitmap> CreateFromBitmapDecoder(
        const wic::BitmapDecoder& bitmap_decoder, 
        std::error_code& error_code);

    static std::shared_ptr<Bitmap> CreateFromBitmapDecoder(const wic::BitmapDecoder& decoder) {
        std::error_code error_code;
        auto result = CreateFromBitmapDecoder(decoder, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

public:
    virtual Size GetPixelSize(std::error_code& error_code) = 0;

    Size GetPixelSize() {
        std::error_code error_code;
        auto result = GetPixelSize(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    virtual std::pair<float, float> GetResolution(std::error_code& error_code) = 0;

    std::pair<float, float> GetResolution() {
        std::error_code error_code;
        auto result = GetResolution(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}