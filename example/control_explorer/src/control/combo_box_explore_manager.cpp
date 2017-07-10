#include "control/combo_box_explore_manager.h"
#include <zaf/control/combo_box.h>
#include "operate/combo_box_operate_item_factory.h"

void ComboBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto combo_box = zaf::Create<zaf::ComboBox>();
    combo_box->SetSize(zaf::Size(100, 30));
    auto drop_down_list = combo_box->GetDropDownListBox();
    drop_down_list->AddItemWithText(L"Apple");
    drop_down_list->AddItemWithText(L"Boy");
    drop_down_list->AddItemWithText(L"Cat");
    drop_down_list->AddItemWithText(L"Dog");
    control = combo_box;

    ComboBoxOperateItemFactory operate_item_factory;
    operate_item_factory.SetTargetControl(combo_box);
    operate_items = operate_item_factory.CreateOperateItems();
}
