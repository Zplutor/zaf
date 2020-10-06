#include "manager/list_box_explore_manager.h"
#include <zaf/object/boxing.h>
#include "property/number_property_item.h"

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
        list_box->AddItem(zaf::Box(each_text));
    }

    return list_box;
}


void ListBoxExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateDefaultItemHeightPropertyItem(),
    });
}


std::shared_ptr<PropertyItem> ListBoxExploreManager::CreateDefaultItemHeightPropertyItem() {

    auto list_box = GetListBox();
    return CreateFloatPropertyItem(
        L"Default item height",
        [list_box]() { return list_box->GetDefaultItemHeight(); },
        [list_box](float value) { list_box->SetDefaultItemHeight(value); });
}