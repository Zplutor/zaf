#include "control/text_box_explore_manager.h"
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include "operate/text_box_operate_item_factory.h"

void TextBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetSize(zaf::Size(200, 100));
    text_box->SetText(L"Text box");
    control = text_box;

    TextBoxOperateItemFactory factory;
    factory.SetTargetControl(text_box);
    operate_items = factory.CreateOperateItems();
}
