#include "control/button/button_explore_manager.h"
#include <zaf/control/button.h>
#include <zaf/creation.h>
#include "control/control_operate_panels_creator.h"
#include "control/textual_control_operate_panels_creator.h"

void ButtonExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto button = zaf::Create<zaf::Button>();
    button->SetSize(zaf::Size(100, 30));
    button->SetText(L"Button");
     
    control = button;

    CreateControlOperatePanels(button, operate_panels);
    CreateTextualControlOperatePanels(button, operate_panels);
}