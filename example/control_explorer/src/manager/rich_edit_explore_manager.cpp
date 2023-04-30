#include "manager/rich_edit_explore_manager.h"

std::shared_ptr<zaf::Control> RichEditExploreManager::CreateExploredControl() {

    auto rich_edit = zaf::Create<zaf::RichEdit>();
    rich_edit->SetSize(zaf::Size(200, 100));
    rich_edit->SetText(L"RichEdit");
    return rich_edit;
}
