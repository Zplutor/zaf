#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void BitmapImageParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& reflection_object) {

    auto& image = dynamic_cast<BitmapImage&>(reflection_object);
    image.SetUri(attribute_value);
}


void BitmapImageParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& image = dynamic_cast<BitmapImage&>(reflection_object);

    XamlNodeParseHelper helper(node, reflection_object.GetType());

    auto uri = helper.GetStringProperty(L"Uri");
    if (uri) {
        image.SetUri(*uri);
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        ParseFromAttribute(*content_string, reflection_object);
    }
}

}