#include "manager/combo_box_explore_manager.h"
#include <zaf/object/boxing/boxing.h>

std::shared_ptr<zaf::Control> ComboBoxExploreManager::CreateExploredControl() {

    auto combo_box = zaf::Create<zaf::ComboBox>();
    combo_box->SetSize(zaf::Size(100, 30));
    auto drop_down_list = combo_box->DropDownListBox();
    drop_down_list->AddItem(zaf::Box(std::wstring{ L"Apple" }));
    drop_down_list->AddItem(zaf::Box(std::wstring{ L"Boy" }));
    drop_down_list->AddItem(zaf::Box(std::wstring{ L"Cat" }));
    drop_down_list->AddItem(zaf::Box(std::wstring{ L"Dog" }));
    return combo_box;
}