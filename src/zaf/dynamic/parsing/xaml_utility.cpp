#include <zaf/dynamic/parsing/xaml_utility.h>
#include <zaf/dynamic/parsing/xaml_node.h>

namespace zaf {

std::optional<std::wstring> GetContentStringFromXamlNode(const XamlNode& node) {

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.size() != 1) {
        return std::nullopt;
    }

    const auto& content_node = content_nodes.front();
    if (content_node->Type() != XamlNodeType::Text) {
        return std::nullopt;
    }

    return content_node->Value();
}

}