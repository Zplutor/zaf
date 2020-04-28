#pragma once

#include <filesystem>
#include <zaf/graphic/render_bitmap.h>

namespace zaf {
namespace wic {
class BitmapDecoder;
class BitmapSource;
}

class Image {
public:
    static std::shared_ptr<Image> FromFile(
        const std::filesystem::path& file_path,
        std::error_code& error_code);

    static std::shared_ptr<Image> FromFile(const std::filesystem::path& file_path) {
        std::error_code error_code;
        auto result = FromFile(file_path, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    static std::shared_ptr<Image> FromBitmap(const wic::BitmapSource& wic_bitmap);

    static std::shared_ptr<Image> FromBitmapDecoder(
        const wic::BitmapDecoder& bitmap_decoder,
        std::error_code& error_code);

    static std::shared_ptr<Image> FromBitmapDecoder(const wic::BitmapDecoder& decoder) {
        std::error_code error_code;
        auto result = FromBitmapDecoder(decoder, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

public:
    Image() = default;
    virtual ~Image() = default;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

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

    virtual RenderBitmap CreateRenderBitmap(Renderer& renderer, std::error_code& error_code) = 0;

    RenderBitmap CreateRenderBitmap(Renderer& renderer) {
        std::error_code error_code;
        auto result = CreateRenderBitmap(renderer, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}