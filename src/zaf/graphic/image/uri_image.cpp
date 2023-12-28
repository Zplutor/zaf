#include <zaf/graphic/image/uri_image.h>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_factory.h>

namespace zaf {
namespace {

class URIImageParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        As<URIImage>(object).InitializeWithURI(attribute_value);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        XamlNodeParseHelper helper(node, object.GetType());

        auto uri = helper.GetStringProperty(L"URI");
        if (uri) {
            As<URIImage>(object).InitializeWithURI(*uri);
        }

        auto content_string = helper.GetContentString();
        if (content_string) {
            ParseFromAttribute(*content_string, object);
        }
    }
};

}


ZAF_DEFINE_TYPE(URIImage)
ZAF_DEFINE_TYPE_PARSER(URIImageParser)
ZAF_DEFINE_TYPE_END


URIImage::URIImage() : dpi_(Application::Instance().GetSystemDPI()) {

}


void URIImage::InitializeWithURI(const std::wstring& uri) {

    ZAF_EXPECT(!image_);
    uri_ = uri;
}


void URIImage::ChangeDPI(float dpi) {

    if (dpi_ != dpi) {
        dpi_ = dpi;
        image_.reset();
    }
}


Size URIImage::GetPixelSize() {

    LoadImageIfNot();
    return image_->GetPixelSize();
}


std::pair<float, float> URIImage::GetResolution() {

    LoadImageIfNot();
    return image_->GetResolution();
}


RenderBitmap URIImage::CreateRenderBitmap(Renderer& renderer) {

    LoadImageIfNot();
    return image_->CreateRenderBitmap(renderer);
}


void URIImage::LoadImageIfNot() {

    if (image_) {
        return;
    }

    auto stream = ResourceFactory::Instance().LoadURI(uri_, dpi_);
    image_ = Image::FromStream(stream);
}

}