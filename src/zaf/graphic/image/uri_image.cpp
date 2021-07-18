#include <zaf/graphic/image/uri_image.h>
#include <zaf/base/assert.h>
#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {

ZAF_DEFINE_TYPE(URIImage)
ZAF_DEFINE_TYPE_PARSER(URIImageParser)
ZAF_DEFINE_TYPE_END


void URIImage::SetURI(const std::wstring& uri) {

    if (!image_) {
        uri_ = uri;
    }
    else {
        ZAF_FAIL();
    }
}


Size URIImage::GetPixelSize() {

    CheckInitialize();
    return image_->GetPixelSize();
}


std::pair<float, float> URIImage::GetResolution() {

    CheckInitialize();
    return image_->GetResolution();
}


RenderBitmap URIImage::CreateRenderBitmap(Renderer& renderer) {

    CheckInitialize();
    return image_->CreateRenderBitmap(renderer);
}


void URIImage::CheckInitialize() {

    if (image_) {
        return;
    }

    auto stream = GetResourceManager().LoadUri(uri_);
    image_ = Image::FromStream(stream);
}

}