#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/xaml_node.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {

void BitmapImageParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    ReflectionObject& reflection_object) {

    auto& image = dynamic_cast<BitmapImage&>(reflection_object);
    image.SetUrl(attribute_value);
}


void BitmapImageParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& image = dynamic_cast<BitmapImage&>(reflection_object);

    XamlNodeParseHelper helper(node, reflection_object.GetType());

    auto url = helper.GetStringProperty(L"Url");
    if (url) {
        image.SetUrl(*url);
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        ParseFromAttribute(*content_string, reflection_object);
    }
}

}