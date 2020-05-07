#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/application.h>
#include <zaf/control/control.h>
#include <zaf/graphic/image/bitmap_image.h>
#include <zaf/parsing/parsers/internal/utility.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

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

    auto name = helper.GetStringProperty(L"Name");
    if (name) {
        control.SetName(*name);
    }

    auto x = helper.GetFloatProperty(L"X");
    if (x) {
        control.SetX(*x);
    }

    auto y = helper.GetFloatProperty(L"Y");
    if (y) {
        control.SetY(*y);
    }

    auto position = helper.GetObjectProperty<Point>(L"Position");
    if (position) {
        control.SetPosition(*position);
    }

    auto width = helper.GetFloatProperty(L"Width");
    if (width) {
        control.SetWidth(*width);
    }

    auto height = helper.GetFloatProperty(L"Height");
    if (height) {
        control.SetHeight(*height);
    }

    auto size = helper.GetObjectProperty<Size>(L"Size");
    if (size) {
        control.SetSize(*size);
    }

    auto rect = helper.GetObjectProperty<Rect>(L"Rect");
    if (rect) {
        control.SetRect(*rect);
    }

    auto minimum_width = helper.GetFloatProperty(L"MinimumWidth");
    if (minimum_width) {
        control.SetMinimumWidth(*minimum_width);
    }

    auto maximum_width = helper.GetFloatProperty(L"MaximumWidth");
    if (maximum_width) {
        control.SetMaximumWidth(*maximum_width);
    }

    auto minimum_height = helper.GetFloatProperty(L"MinimumHeight");
    if (minimum_height) {
        control.SetMinimumHeight(*minimum_height);
    }

    auto maximum_height = helper.GetFloatProperty(L"MaximumHeight");
    if (maximum_height) {
        control.SetMaximumHeight(*maximum_height);
    }

    auto fixed_width = helper.GetFloatProperty(L"FixedWidth");
    if (fixed_width) {
        control.SetFixedWidth(*fixed_width);
    }

    auto fixed_height = helper.GetFloatProperty(L"FixedHeight");
    if (fixed_height) {
        control.SetFixedHeight(*fixed_height);
    }

    auto fixed_size = helper.GetObjectProperty<Size>(L"FixedSize");
    if (fixed_size) {
        control.SetFixedSize(*fixed_size);
    }

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

    auto is_visible = helper.GetBoolProperty(L"IsVisible");
    if (is_visible) {
        control.SetIsVisible(*is_visible);
    }

    auto is_enabled = helper.GetBoolProperty(L"IsEnabled");
    if (is_enabled) {
        control.SetIsEnabled(*is_enabled);
    }

    auto can_focused = helper.GetBoolProperty(L"CanFocused");
    if (can_focused) {
        control.SetCanFocused(*can_focused);
    }

    auto can_tab_stop = helper.GetBoolProperty(L"CanTabStop");
    if (can_tab_stop) {
        control.SetCanTabStop(*can_tab_stop);
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

    auto layouter_object = helper.GetDynamicObjectProperty<ReflectionObject>(L"Layouter");
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
        if (child_control == nullptr) {
            continue;
        }

        control.AddChild(child_control);
    }
}

}

void ControlParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& control = dynamic_cast<Control&>(reflection_object);
    Control::UpdateGuard update_guard(control);

    ParseProperties(node, control);
    ParseChildren(node, control);
}

}