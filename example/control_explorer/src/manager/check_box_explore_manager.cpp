#include "manager/check_box_explore_manager.h"

std::shared_ptr<zaf::Control> CheckBoxExploreManager::CreateExploredControl() {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(200, 30));
    check_box->SetText(L"Check box");
    return check_box;
}
