#include <zaf/object/style/sheet_parser.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/style/sheet.h>

namespace zaf {

void SheetParser::ParseFromNode(const XamlNode& node, Object& object) {

    if (!node.GetContentNodes().empty()) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    auto& sheet = As<Sheet>(object);

    for (const auto& each_attribute : node.GetAttributes()) {
        sheet.AddDeclaration(each_attribute->Name(), each_attribute->Value());
    }
}

}