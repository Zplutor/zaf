#include "manager/text_box_explore_manager.h"

std::shared_ptr<zaf::Control> TextBoxExploreManager::CreateExploredControl() {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetSize(zaf::Size(200, 100));
    text_box->SetText(L"Text box");
    return text_box;
}
