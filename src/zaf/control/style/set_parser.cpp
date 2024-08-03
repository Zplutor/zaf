#include <zaf/control/style/set_parser.h>
#include <zaf/base/as.h>
#include <zaf/control/style/set.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>

namespace zaf {

void SetParser::ParseFromNode(const XamlNode& node, Object& object) {

    if (!node.GetContentNodes().empty()) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    auto& set = As<Set>(object);

    for (const auto& each_attribute : node.GetAttributes()) {
        set.AddProperty(each_attribute->Name(), each_attribute->Value());
    }
}

}