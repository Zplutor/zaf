#include <zaf/parsing/parsers/bitmap_image_parser.h>
#include <zaf/graphic/image/uri_image.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void URIImageParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& reflection_object) {

    auto& image = dynamic_cast<URIImage&>(reflection_object);
    image.SetURI(attribute_value);
}


void URIImageParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& image = dynamic_cast<URIImage&>(reflection_object);

    XamlNodeParseHelper helper(node, reflection_object.GetType());

    auto uri = helper.GetStringProperty(L"Uri");
    if (uri) {
        image.SetURI(*uri);
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        ParseFromAttribute(*content_string, reflection_object);
    }
}

}