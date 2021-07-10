#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/control/control.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<ImageLayout> ParseImageLayout(const std::wstring& value) {

    if (value == L"Center") {
        return ImageLayout::Center;
    }
    else if (value == L"Tile") {
        return ImageLayout::Tile;
    }
    else if (value == L"Zoom") {
        return ImageLayout::Zoom;
    }
    else if (value == L"Stretch") {
        return ImageLayout::Stretch;
    }
    else if (value == L"None") {
        return ImageLayout::None;
    }
    else {
        return {};
    }
}


void ParseProperties(const XamlNode& node, Control& control) {

    XamlNodeParseHelper helper(node, control.GetType());

    auto margin = helper.GetObjectProperty<Frame>(L"Margin");
    if (margin) {
        control.SetMargin(*margin);
    }

    auto border = helper.GetObjectProperty<Frame>(L"Border");
    if (border) {
        control.SetBorder(*border);
    }

    auto padding = helper.GetObjectProperty<Frame>(L"Padding");
    if (padding) {
        control.SetPadding(*padding);
    }

    auto tab_index = helper.GetFloatProperty(L"TabIndex");
    if (tab_index) {
        control.SetTabIndex(static_cast<std::size_t>(*tab_index));
    }

    auto background_color = helper.GetObjectProperty<Color>(L"BackgroundColor");
    if (background_color) {
        control.SetBackgroundColor(*background_color);
    }

    auto border_color = helper.GetObjectProperty<Color>(L"BorderColor");
    if (border_color) {
        control.SetBorderColor(*border_color);
    }

    auto layouter_object = helper.GetDynamicObjectProperty<Object>(L"Layouter");
    if (layouter_object) {
        auto layouter = std::dynamic_pointer_cast<Layouter>(layouter_object);
        if (layouter) {
            control.SetLayouter(layouter);
        }
    }

    auto background_image = helper.GetObjectProperty<BitmapImage>(L"BackgroundImage");
    if (background_image) {
        control.SetBackgroundImage(background_image);
    }

    auto background_image_layout_string = helper.GetStringProperty(L"BackgroundImageLayout");
    if (background_image_layout_string) {
        auto background_image_layout = ParseImageLayout(*background_image_layout_string);
        if (background_image_layout) {
            control.SetBackgroundImageLayout(*background_image_layout);
        }
    }
}


void ParseChildren(const XamlNode& node, Control& control) {

    for (const auto& each_node : node.GetContentNodes()) {

        auto child_control = internal::CreateObjectFromNode<Control>(each_node);
        if (!child_control) {
            ZAF_THROW_SYSTEM_ERROR(ERROR_INVALID_NAME);
        }

        control.AddChild(child_control);
    }
}

}

void ControlParser::ParseFromNode(const XamlNode& node, Object& object) {

    auto& control = dynamic_cast<Control&>(object);
    auto update_guard = control.BeginUpdate();

    __super::ParseFromNode(node, object);

    ParseProperties(node, control);
    ParseChildren(node, control);
}

}