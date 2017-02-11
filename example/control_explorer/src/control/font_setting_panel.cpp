#include "control/font_setting_panel.h"
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/property/setting.h>
#include <zaf/property/text_box_property_tag.h>

void FontSettingPanel::Initialize() {

    __super::Initialize();

    SetLayouter(zaf::GetVerticalArrayLayouter());

    InitializeFamilyNamePanel();
    InitializeSizePanel();
    InitializeWeightPanel();
}


void FontSettingPanel::InitializeFamilyNamePanel() {

    family_name_combo_box_ = zaf::Create<zaf::ComboBox>();
    family_name_combo_box_->SetMaximumVisibleItemCount(10);

    auto font_name_drop_down_list = family_name_combo_box_->GetDropDownListBox();
    font_name_drop_down_list->AddItemWithText(std::wstring());

    wchar_t locale_name[1024] = { 0 };
    LCIDToLocaleName(LANG_USER_DEFAULT, locale_name, 1024, 0);
    std::wstring user_default_locale = locale_name;

    auto font_collection = zaf::GetResourceFactory()->GetSystemFontCollection();

    for (const auto& each_font_family : font_collection.GetEnumerator()) {

        auto localized_strings = each_font_family.GetFamilyNames();
        auto index = localized_strings.FindLocaleName(user_default_locale);
        if (index == zaf::InvalidIndex) {
            index = localized_strings.FindLocaleName(L"en-us");
        }
        if (index != zaf::InvalidIndex) {
            font_name_drop_down_list->AddItemWithText(localized_strings.GetString(index));
        }
    }

    auto panel = zaf::CreateWithProperties<zaf::Control>(
        zaf::layouter = zaf::GetHorizontalArrayLayouter(),
        zaf::child = zaf::CreateWithProperties<zaf::Label>(zaf::text = L"Family name"),
        zaf::child = family_name_combo_box_
    );

    AddChild(panel);
}


void FontSettingPanel::InitializeSizePanel() {

    auto panel = zaf::CreateWithProperties<zaf::Control>(
        zaf::layouter = zaf::GetHorizontalArrayLayouter(),
        zaf::child = zaf::CreateWithProperties<zaf::Label>(zaf::text = L"Size"),
        zaf::child = size_text_box_ = zaf::CreateWithProperties<zaf::TextBox>(
            zaf::textValidator = zaf::GetNumberTextValidator()
        )
    );

    AddChild(panel);
}


void FontSettingPanel::InitializeWeightPanel() {

    auto panel = zaf::CreateWithProperties<zaf::Control>(
        zaf::layouter = zaf::GetHorizontalArrayLayouter(),
        zaf::child = zaf::CreateWithProperties<zaf::Label>(zaf::text = L"Weight"),
        zaf::child = weight_text_box_ = zaf::CreateWithProperties<zaf::TextBox>(
            zaf::textValidator = zaf::GetNumberTextValidator()
        )
    );

    AddChild(panel);
}


zaf::Font FontSettingPanel::GetFont() const {

    zaf::Font font = zaf::Font::GetDefault();

    auto family_name = family_name_combo_box_->GetText();
    if (! family_name.empty()) {
        font.family_name = family_name;
    }

    auto size = size_text_box_->GetText();
    if (! size.empty()) {
        font.size = zaf::ToNumeric<float>(size);
    }

    auto weight = zaf::ToNumeric<int>(weight_text_box_->GetText());
    font.weight = std::max(zaf::FontWeight::Minimum, std::min(zaf::FontWeight::Maximum, weight));

    return font;
}


void FontSettingPanel::SetFont(const zaf::Font& font) {

    auto drop_down_list_box = family_name_combo_box_->GetDropDownListBox();
    for (std::size_t index = 0; index < drop_down_list_box->GetItemCount(); ++index) {

        if (font.family_name == drop_down_list_box->GetItemTextAtIndex(index)) {
            drop_down_list_box->SelectItemAtIndex(index);
            break;
        }
    }

    size_text_box_->SetText(zaf::ToWideString(font.size));
    weight_text_box_->SetText(zaf::ToWideString(font.weight));
}