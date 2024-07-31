#include <zaf/object/style/selector_parser.h>
#include <zaf/base/as.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/style/selector.h>

namespace zaf {

void SelectorParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto& selector = As<Selector>(object);

    for (const auto& each_attribute : node.GetAttributes()) {
        selector.AddDeclaration(each_attribute->Name(), each_attribute->Value());
    }

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.empty()) {
        return;
    }

    if (content_nodes.size() > 1) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    auto sheet = internal::CreateObjectFromNode<Sheet>(content_nodes.front());
    if (!sheet) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    selector.SetSheet(std::move(sheet));
}

}