#include "manager/text_box_explore_manager.h"
#include <zaf/control/text_box.h>

std::shared_ptr<zaf::Control> TextBoxExploreManager::CreateExploredControl() {

    auto result = zaf::Create<zaf::TextBox>();
    result->SetSize(zaf::Size(200, 100));
    result->SetText(L"TextBox");
    return result;
}
