#include "control/color_setting_panel.h"
#include <zaf/control/layout/array_layouter.h>

struct ColorMapItem {
    const wchar_t* name;
    zaf::Color color;
};

const ColorMapItem* GetColorMap() {

    static ColorMapItem map[] = {

        { L"Black", zaf::Color::Black },
        { L"White", zaf::Color::White },
        { L"Red", zaf::Color::Red },
        { L"Green", zaf::Color::Green },
        { L"Blue", zaf::Color::Blue },
        { L"Yellow", zaf::Color::Yellow },
        { nullptr },
    };

    return map;
}


void ColorSettingPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::GetHorizontalArrayLayouter());

    InitializeColorComboBox();
}


void ColorSettingPanel::InitializeColorComboBox() {

    color_combo_box_ = zaf::Create<zaf::ComboBox>();
    auto drop_down_list_box = color_combo_box_->GetDropDownListBox();

    auto color_map = GetColorMap();
    while (color_map->name != nullptr) {
        drop_down_list_box->AddItemWithText(color_map->name);
        ++color_map;
    }

    AddChild(color_combo_box_);
}


const zaf::Color ColorSettingPanel::GetColor() const {

    auto color_name = color_combo_box_->GetText();

    auto color_map = GetColorMap();
    while (color_map->name != nullptr) {
        if (color_map->name == color_name) {
            return color_map->color;
        }
        ++color_map;
    }

    return zaf::Color();
}


void ColorSettingPanel::SetColor(const zaf::Color& color) {

    auto color_map = GetColorMap();
    while (color_map->name != nullptr) {
        if (color_map->color == color) {
            color_combo_box_->SetText(color_map->name);
            break;
        }
        ++color_map;
    }
}