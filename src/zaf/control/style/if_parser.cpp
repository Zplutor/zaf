#include <zaf/control/style/if_parser.h>
#include <zaf/base/as.h>
#include <zaf/control/style/if.h>
#include <zaf/object/parsing/internal/utility.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>

namespace zaf {

void IfParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto& if_statement = As<If>(object);

    for (const auto& each_attribute : node.GetAttributes()) {
        if_statement.AddCondition(each_attribute->Name(), each_attribute->Value());
    }

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.empty()) {
        return;
    }

    if (content_nodes.size() > 1) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    auto set = internal::CreateObjectFromNode<Set>(content_nodes.front());
    if (!set) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    if_statement.SetResult(std::move(set));
}

}