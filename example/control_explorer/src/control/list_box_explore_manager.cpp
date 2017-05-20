#include "control/list_box_explore_manager.h"
#include <zaf/control/list_box.h>
#include "operate/list_box_operate_item_factory.h"

static std::shared_ptr<zaf::Control> CreateItemHeightOperatePanel(const std::shared_ptr<zaf::ListBox>& list_box);

void ListBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

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
        list_box->AddItemWithText(each_text);
    }
    
    control = list_box;

    ListBoxOperateItemFactory factory;
    factory.SetTargetControl(list_box);
    operate_items = factory.CreateOperateItems();
}