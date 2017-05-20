#include "control/check_box_explore_manager.h"
#include <zaf/control/check_box.h>
#include <zaf/creation.h>
#include "operate/check_box_operate_item_factory.h"

void CheckBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(200, 30));
    check_box->SetText(L"Check box");
    control = check_box;

    CheckBoxOperateItemFactory factory;
    factory.SetTargetControl(check_box);
    operate_items = factory.CreateOperateItems();
}