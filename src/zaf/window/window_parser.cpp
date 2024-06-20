#include <zaf/window/window_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/window/window.h>

namespace zaf {

void WindowParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    __super::ParseFromNode(node, reflection_object);

    auto& window = dynamic_cast<Window&>(reflection_object);
    ParseControls(node, window);
}


void WindowParser::ParseControls(const XamlNode& node, Window& window) {

    std::vector<std::shared_ptr<Control>> controls;

    for (const auto& each_node : node.GetContentNodes()) {

        auto control = internal::CreateObjectFromNode<Control>(each_node);
        if (control == nullptr) {
            continue;
        }

        controls.push_back(control);
    }

    if (!controls.empty()) {
        window.RootControl()->AddChildren(controls);
    }
}

}