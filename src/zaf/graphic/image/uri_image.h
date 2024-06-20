#pragma once

#include <zaf/graphic/image/image.h>
#include <zaf/graphic/image/uri_image_parser.h>
#include <zaf/object/object.h>

namespace zaf {

class URIImage : public Image, public Object {
public:
    ZAF_OBJECT;

public:
    URIImage();

    void InitializeWithURI(const std::wstring& uri);

    void ChangeDPI(float dpi);

    Size GetPixelSize() override;
    std::pair<float, float> GetResolution() override;
    RenderBitmap CreateRenderBitmap(Renderer& renderer) override;

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

}