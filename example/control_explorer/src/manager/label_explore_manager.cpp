#include "manager/label_explore_manager.h"
#include <zaf/control/label.h>

std::shared_ptr<zaf::Control> LabelExploreManager::CreateExploredControl() {

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Label");
    label->SetSize(zaf::Size(200, 30));
    return label;
}