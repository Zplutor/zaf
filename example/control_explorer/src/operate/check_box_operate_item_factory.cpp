#include "operate/check_box_operate_item_factory.h"
#include "operate/operate_item_creation.h"

void CheckBoxOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateCanAutoChangeCheckStateOperateItem());
    items.push_back(CreateCanBeIndeterminateOperateItem());
    items.push_back(CreateIsCheckedOperateItem());
    items.push_back(CreateCheckStateOperateItem());
}


std::shared_ptr<zaf::Control> CheckBoxOperateItemFactory::CreateCanAutoChangeCheckStateOperateItem() {

    auto check_box = GetCheckBox();

    return CreateOperateItemWithCheckBox(
        L"Can auto check",
        [check_box]() {
            return check_box->CanAutoChangeCheckState();
        },
        [check_box](bool value) {
            check_box->SetCanAutoChangeCheckState(value);
        },
        nullptr);
}


std::shared_ptr<zaf::Control> CheckBoxOperateItemFactory::CreateCanBeIndeterminateOperateItem() {

    auto check_box = GetCheckBox();

    return CreateOperateItemWithCheckBox(
        L"Can be indeterminate",
        [check_box]() {
            return check_box->CanBeIndeterminate();
        },
        [check_box](bool value) { 
            check_box->SetCanBeIndeterminate(value);
        }, 
        nullptr);
}


std::shared_ptr<zaf::Control> CheckBoxOperateItemFactory::CreateIsCheckedOperateItem() {

    auto check_box = GetCheckBox();

    return CreateOperateItemWithCheckBox(
        L"Is checked",
        [check_box]() { return check_box->IsChecked(); },
        [check_box](bool value) { check_box->SetIsChecked(value); },
        [check_box](const std::function<void()>& callback) {
            check_box->GetCheckStateChangeEvent().AddListener(std::bind(callback));
        });
}


std::shared_ptr<zaf::Control> CheckBoxOperateItemFactory::CreateCheckStateOperateItem() {

    auto check_box = GetCheckBox();

    return CreateOperateItemWithComboBox<zaf::CheckState>(
        L"Check state",
        {
            { L"Checked", zaf::CheckState::Checked },
            { L"Indeterminate", zaf::CheckState::Indeterminate },
            { L"Unchecked", zaf::CheckState::Unchecked },
        },
        [check_box]() { return check_box->GetCheckState(); },
        [check_box](zaf::CheckState value) { check_box->SetCheckState(value); },
        [check_box](const std::function<void()>& callback) { 
            check_box->GetCheckStateChangeEvent().AddListener(std::bind(callback));
        });
}