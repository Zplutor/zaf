#include "manager/list_box_explore_manager.h"
#include <zaf/dynamic/boxing/boxing.h>

std::shared_ptr<zaf::Control> ListBoxExploreManager::CreateExploredControl() {

    auto list_box = zaf::Create<zaf::ListBox>();
    list_box->SetSize(zaf::Size(200, 200));

    auto texts = {
        L"Apple",
        L"Boy",
        L"Cat",
        L"Dog",
        L"Egg",
        L"Food",
        L"God",
    };

    for (auto each_text : texts) {
        list_box->AddItem(zaf::Box(std::wstring{ each_text }));
    }

    return list_box;
}
