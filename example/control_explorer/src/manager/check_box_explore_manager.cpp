#include "manager/check_box_explore_manager.h"
#include "property/check_box_property_item.h"
#include "property/combo_box_property_item.h"

std::shared_ptr<zaf::Control> CheckBoxExploreManager::CreateExploredControl() {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(200, 30));
    check_box->SetText(L"Check box");
    return check_box;
}


void CheckBoxExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateCanAutoChangeCheckStatePropertyItem(),
        CreateCanBeIndeterminatePropertyItem(),
        CreateIsCheckedPropertyItem(),
        CreateCheckStatePropertyItem(),
    });
}


std::shared_ptr<PropertyItem> CheckBoxExploreManager::CreateCanAutoChangeCheckStatePropertyItem() {

    auto check_box = GetCheckBox();

    return CreateCheckBoxPropertyItem(
        L"Can auto check",
        [check_box]() {
            return check_box->CanAutoChangeCheckState();
        },
        [check_box](bool value) {
            check_box->SetCanAutoChangeCheckState(value);
        },
        nullptr);
}


std::shared_ptr<PropertyItem> CheckBoxExploreManager::CreateCanBeIndeterminatePropertyItem() {

    auto check_box = GetCheckBox();

    return CreateCheckBoxPropertyItem(
        L"Can be indeterminate",
        [check_box]() {
            return check_box->CanBeIndeterminate();
        },
        [check_box](bool value) { 
            check_box->SetCanBeIndeterminate(value);
        }, 
        nullptr);
}


std::shared_ptr<PropertyItem> CheckBoxExploreManager::CreateIsCheckedPropertyItem() {

    auto check_box = GetCheckBox();

    return CreateCheckBoxPropertyItem(
        L"Is checked",
        [check_box]() { return check_box->IsChecked(); },
        [check_box](bool value) { check_box->SetIsChecked(value); },
        [check_box](const std::function<void()>& callback) {
            check_box->Subscriptions() +=
                check_box->CheckStateChangeEvent().Subscribe(std::bind(callback));
        });
}


std::shared_ptr<PropertyItem> CheckBoxExploreManager::CreateCheckStatePropertyItem() {

    auto check_box = GetCheckBox();

    return CreateComboBoxPropertyItem<zaf::CheckState>(
        L"Check state",
        {
            { L"Checked", zaf::CheckState::Checked },
            { L"Indeterminate", zaf::CheckState::Indeterminate },
            { L"Unchecked", zaf::CheckState::Unchecked },
        },
        [check_box]() { return check_box->GetCheckState(); },
        [check_box](zaf::CheckState value) { check_box->SetCheckState(value); },
        [check_box](const std::function<void()>& callback) { 
            check_box->Subscriptions() +=
                check_box->CheckStateChangeEvent().Subscribe(std::bind(callback));
        });
}


