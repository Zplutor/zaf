#include "operate/radio_button_operate_item_factory.h"
#include "operate/operate_item_creation.h"

void RadioButtonOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateCanAutoSelectOperateItem());
    items.push_back(CreateIsSelectedOperateItem());
}


std::shared_ptr<zaf::Control> RadioButtonOperateItemFactory::CreateCanAutoSelectOperateItem() {

    auto radio_button = GetRadioButton();

    return CreateOperateItemWithCheckBox(
        L"Can auto select",
        [radio_button]() { return radio_button->CanAutoSelect(); },
        [radio_button](bool value) { radio_button->SetCanAutoSelect(value); },
        nullptr);
}


std::shared_ptr<zaf::Control> RadioButtonOperateItemFactory::CreateIsSelectedOperateItem() {

    auto radio_button = GetRadioButton();

    return CreateOperateItemWithCheckBox(
        L"Is selected",
        [radio_button]() { return radio_button->IsSelected(); },
        [radio_button](bool value) { if (value) { radio_button->SetSelected(); } },
        [radio_button](const std::function<void()>& callback) {
            radio_button->GetSelectStateChangeEvent().AddListener(std::bind(callback));
        }
    );
}