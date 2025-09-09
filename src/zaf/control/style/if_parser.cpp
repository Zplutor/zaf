#include <zaf/control/style/if_parser.h>
#include <zaf/base/as.h>
#include <zaf/control/style/if.h>
#include <zaf/dynamic/parsing/internal/utility.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/parsing/xaml_node.h>

namespace zaf {

void IfParser::ParseFromNode(const XamlNode& node, dynamic::Object& object) {

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

    auto result = internal::CreateObjectFromNode<dynamic::Object>(content_nodes.front());
    if (!result) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    if_statement.SetResult(std::move(result));
}

}