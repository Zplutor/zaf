#include <zaf/graphic/image/uri_image.h>
#include <zaf/base/assert.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_manager.h>

namespace zaf {
namespace {

class URIImageParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

        auto& image = dynamic_cast<URIImage&>(object);
        image.SetURI(attribute_value);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        auto& image = dynamic_cast<URIImage&>(object);

        XamlNodeParseHelper helper(node, object.GetType());

        auto uri = helper.GetStringProperty(L"Uri");
        if (uri) {
            image.SetURI(*uri);
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