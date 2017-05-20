#include "label_explore_manager.h"
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include "operate/textual_control_operate_item_factory.h"

void LabelExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Label");
    label->SetSize(zaf::Size(200, 30));
    control = label;

    TextualControlOperateItemFactory factory;
    factory.SetTargetControl(label);
    operate_items = factory.CreateOperateItems();
}