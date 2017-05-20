#include "operate/color_combo_box.h"

static const struct {
    const wchar_t* name;
    zaf::Color color;
} g_color_map[] = {

    { L"Black", zaf::Color::Black },
    { L"Blue", zaf::Color::Blue },
    { L"Green", zaf::Color::Green },
    { L"Red", zaf::Color::Red },
    { L"White", zaf::Color::White },
    { L"Yellow", zaf::Color::Yellow },
};

void ColorComboBox::Initialize() {

    __super::Initialize();

    auto drop_down_list_box = GetDropDownListBox();

    for (const auto& each_item : g_color_map) {
        drop_down_list_box->AddItemWithText(each_item.name);
    }
}


zaf::Color ColorComboBox::GetColor() const {
    return{};
}


void ColorComboBox::SetColor(const zaf::Color& color) {

    
}