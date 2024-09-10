#include <zaf/graphic/uri_image_parser.h>
#include <zaf/graphic/image.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

void URIImageParser::ParseFromAttribute(const std::wstring& attribute_value, Object& object) {
    As<URIImage>(object).InitializeWithURI(attribute_value);
}


void URIImageParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    XamlNodeParseHelper helper(node, object.DynamicType());

    auto uri = helper.GetStringProperty(L"URI");
    if (uri) {
        As<URIImage>(object).InitializeWithURI(*uri);
    }

    auto content_string = helper.GetContentString();
    if (content_string) {
        ParseFromAttribute(*content_string, object);
    }
}

}