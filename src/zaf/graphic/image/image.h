#pragma once

#include <filesystem>
#include <zaf/graphic/render_bitmap.h>

namespace zaf {
namespace wic {
class BitmapDecoder;
class BitmapSource;
}

class Stream;

class Image {
public:
    static std::shared_ptr<Image> FromFile(const std::filesystem::path& file_path);

    static std::shared_ptr<Image> FromURI(const std::wstring& uri);
    static std::shared_ptr<Image> FromURI(const std::wstring& uri, float dpi);

    static std::shared_ptr<Image> FromBitmap(const wic::BitmapSource& wic_bitmap);

    static std::shared_ptr<Image> FromBitmapDecoder(const wic::BitmapDecoder& bitmap_decoder);

    static std::shared_ptr<Image> FromStream(const Stream& stream);

public:
    Image() = default;
    virtual ~Image() = default;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    virtual Size GetPixelSize() = 0;

    virtual std::pair<float, float> GetResolution() = 0;

    virtual RenderBitmap CreateRenderBitmap(Renderer& renderer) = 0;
};

}