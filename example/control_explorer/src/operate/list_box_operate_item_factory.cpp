#include "operate/list_box_operate_item_factory.h"
#include "operate/operate_item_creation.h"

void ListBoxOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateItemHeightOperateItem());
}


std::shared_ptr<zaf::Control> ListBoxOperateItemFactory::CreateItemHeightOperateItem() {

    auto list_box = GetListBox();

    return CreateOperateItemWithFloatTextBox(
        L"Item height",
        [list_box]() { return list_box->GetItemHeight(); },
        [list_box](float value) { list_box->SetItemHeight(value); });
}