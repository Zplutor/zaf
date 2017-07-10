#include "operate/combo_box_operate_item_factory.h"
#include "operate/operate_item_creation.h"

void ComboBoxOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateIsEditableOperateItem());
    items.push_back(CreateDropDownButtonWidthOperateItem());
    items.push_back(CreateMinimumVisibleItemCountOperateItem());
    items.push_back(CreateMaximumVisibleItemCountOperateItem());
}


std::shared_ptr<zaf::Control> ComboBoxOperateItemFactory::CreateIsEditableOperateItem() {

    auto combo_box = GetComboBox();
    return CreateOperateItemWithCheckBox(
        L"Is editable",
        [combo_box]() { return combo_box->IsEditable(); },
        [combo_box](bool value) { combo_box->SetIsEditable(value); }, 
        nullptr);
}


std::shared_ptr<zaf::Control> ComboBoxOperateItemFactory::CreateDropDownButtonWidthOperateItem() {

    auto combo_box = GetComboBox();
    return CreateOperateItemWithFloatTextBox(
        L"Drop down button width",
        [combo_box]() { return combo_box->GetDropDownButtonWidth(); },
        [combo_box](float value) { combo_box->SetDropDownButtonWidth(value); });
}


std::shared_ptr<zaf::Control> ComboBoxOperateItemFactory::CreateMinimumVisibleItemCountOperateItem() {

    auto combo_box = GetComboBox();
    return CreateOperateItemWithIntegerTextBox(
        L"Minimum visible item count", 
        [combo_box]() { return combo_box->GetMinimumVisibleItemCount();},
        [combo_box](std::int64_t value) { combo_box->SetMinimumVisibleItemCount(static_cast<std::size_t>(value)); });
}


std::shared_ptr<zaf::Control> ComboBoxOperateItemFactory::CreateMaximumVisibleItemCountOperateItem() {

    auto combo_box = GetComboBox();
    return CreateOperateItemWithIntegerTextBox(
        L"Maximum visible item count",
        [combo_box]() { return combo_box->GetMaximumVisibleItemCount(); },
        [combo_box](std::int64_t value) { combo_box->SetMaximumVisibleItemCount(static_cast<std::size_t>(value)); });
}