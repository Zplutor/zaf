#include <zaf/object/style/style_parser.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/style/selector.h>
#include <zaf/object/style/sheet.h>
#include <zaf/object/style/style.h>

namespace zaf {

void StyleParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto& style = As<Style>(object);

    for (const auto& each_content_node : node.GetContentNodes()) {

        auto selector = internal::CreateObjectFromNode<Selector>(each_content_node);
        if (selector) {
            style.AddSelector(std::move(selector));
            continue;
        }

        auto sheet = internal::CreateObjectFromNode<Sheet>(each_content_node);
        if (sheet) {
            style.SetSheet(std::move(sheet));
            continue;
        }

        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}

}