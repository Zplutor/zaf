#include <zaf/control/style/color_picker_parser.h>
#include <zaf/control/style/color_picker.h>
#include <zaf/control/style/if.h>
#include <zaf/dynamic/parsing/internal/utility.h>
#include <zaf/internal/control/color_picker_impl.h>

namespace zaf {

void ColorPickerParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    internal::ColorPickerImpl impl;

    for (const auto& each_node : node.GetContentNodes()) {

        auto if_object = internal::CreateObjectFromNode<If>(each_node);
        if (if_object) {
            impl.AddPickObject(if_object);
            continue;
        }

        auto color = internal::CreateObjectFromNode<Color>(each_node);
        if (color) {
            impl.AddPickObject(color);
            continue;
        }

        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    As<ColorPicker>(object) = std::move(impl);
}

}