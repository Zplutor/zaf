#pragma once

#include <filesystem>
#include <zaf/graphic/render_bitmap.h>
#include <zaf/graphic/image/uri_image_parser.h>
#include <zaf/object/custom_property_value_traits.h>
#include <zaf/object/object.h>

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

    virtual RenderBitmap CreateRenderBitmap(d2d::Renderer& renderer) = 0;
};


class URIImage : public Image, public Object {
public:
    ZAF_OBJECT;

public:
    URIImage();

    void InitializeWithURI(const std::wstring& uri);

    void ChangeDPI(float dpi);

    Size GetPixelSize() override;
    std::pair<float, float> GetResolution() override;
    RenderBitmap CreateRenderBitmap(d2d::Renderer& renderer) override;

    const std::wstring& GetURI() const {
        return uri_;
    }

private:
    void LoadImageIfNot();

private:
    std::wstring uri_;
    float dpi_{};
    std::shared_ptr<Image> image_;
};

ZAF_OBJECT_BEGIN(URIImage);
ZAF_OBJECT_PARSER(URIImageParser);
ZAF_OBJECT_END;


template<>
struct zaf__CustomPropertyValueTraits<std::shared_ptr<Image>> {

    using BoxedType = URIImage;

    static std::shared_ptr<BoxedType> ToBoxedObject(const std::shared_ptr<Image>& image) {
        return std::dynamic_pointer_cast<URIImage>(image);
    }

    static std::shared_ptr<Image> FromBoxedObject(const std::shared_ptr<BoxedType>& value) {
        return std::dynamic_pointer_cast<Image>(value);
    }
};

}