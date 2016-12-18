#include "label_explore_manager.h"
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include "control/control_operate_panels_creator.h"
#include "control/textual_control_operate_panels_creator.h"

void LabelExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Label");
    label->SetSize(zaf::Size(200, 30));
    control = label;

    CreateControlOperatePanels(label, operate_panels);
    CreateTextualControlOperatePanels(label, operate_panels);
}

