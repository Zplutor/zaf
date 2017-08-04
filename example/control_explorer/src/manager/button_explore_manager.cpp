#include "manager/button_explore_manager.h"
#include <zaf/control/button.h>

std::shared_ptr<zaf::Control> ButtonExploreManager::CreateExploredControl() {

    auto button = zaf::Create<zaf::Button>();
    button->SetSize(zaf::Size(100, 30));
    button->SetText(L"Button");
    return button;
}