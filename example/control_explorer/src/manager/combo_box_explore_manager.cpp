#include "manager/combo_box_explore_manager.h"
#include <zaf/object/boxing.h>
#include "property/check_box_property_item.h"
#include "property/number_property_item.h"

std::shared_ptr<zaf::Control> ComboBoxExploreManager::CreateExploredControl() {

    auto combo_box = zaf::Create<zaf::ComboBox>();
    combo_box->SetSize(zaf::Size(100, 30));
    auto drop_down_list = combo_box->GetDropDownListBox();
    drop_down_list->AddItem(zaf::Box(L"Apple"));
    drop_down_list->AddItem(zaf::Box(L"Boy"));
    drop_down_list->AddItem(zaf::Box(L"Cat"));
    drop_down_list->AddItem(zaf::Box(L"Dog"));
    return combo_box;
}


void ComboBoxExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateIsEditablePropertyItem(),
        CreateDropDownButtonWidthPropertyItem(),
        CreateMinimumVisibleItemCountPropertyItem(),
        CreateMaximumVisibleItemCountPropertyItem(),
    });
}


std::shared_ptr<PropertyItem> ComboBoxExploreManager::CreateIsEditablePropertyItem() {

    auto combo_box = GetComboBox();
    return CreateCheckBoxPropertyItem(
        L"Is editable",
        [combo_box]() { return combo_box->IsEditable(); },
        [combo_box](bool value) { combo_box->SetIsEditable(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> ComboBoxExploreManager::CreateDropDownButtonWidthPropertyItem() {

    auto combo_box = GetComboBox();
    return CreateFloatPropertyItem(
        L"Drop down button width",
        [combo_box]() { return combo_box->GetDropDownButtonWidth(); },
        [combo_box](float value) { combo_box->SetDropDownButtonWidth(value); });
}


std::shared_ptr<PropertyItem> ComboBoxExploreManager::CreateMinimumVisibleItemCountPropertyItem() {

    auto combo_box = GetComboBox();
    return CreateIntegerPropertyItem(
        L"Minimum visible item count",
        [combo_box]() { return combo_box->GetMinVisibleItemCount(); },
        [combo_box](std::int64_t value) { combo_box->SetMinVisibleItemCount(static_cast<std::size_t>(value)); });
}


std::shared_ptr<PropertyItem> ComboBoxExploreManager::CreateMaximumVisibleItemCountPropertyItem() {

    auto combo_box = GetComboBox();
    return CreateIntegerPropertyItem(
        L"Maximum visible item count",
        [combo_box]() { return combo_box->GetMaxVisibleItemCount(); },
        [combo_box](std::int64_t value) { combo_box->SetMaxVisibleItemCount(static_cast<std::size_t>(value)); });
}