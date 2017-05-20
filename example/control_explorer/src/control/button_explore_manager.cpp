#include "control/button_explore_manager.h"
#include <zaf/control/button.h>
#include <zaf/creation.h>
#include "operate/textual_control_operate_item_factory.h"

void ButtonExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto button = zaf::Create<zaf::Button>();
    button->SetSize(zaf::Size(100, 30));
    button->SetText(L"Button");
    control = button;

    TextualControlOperateItemFactory factory;
    factory.SetTargetControl(button);
    operate_items = factory.CreateOperateItems();
}