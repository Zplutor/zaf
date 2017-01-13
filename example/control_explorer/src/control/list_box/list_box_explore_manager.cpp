#include "control/list_box/list_box_explore_manager.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/button.h>
#include <zaf/control/label.h>
#include <zaf/control/list_box.h>
#include <zaf/control/text_box.h>
#include <zaf/property/clickable_control_property_tag.h>
#include <zaf/property/control_property_tag.h>
#include <zaf/property/list_box_property_tag.h>
#include <zaf/property/text_box_property_tag.h>
#include <zaf/property/textual_control_property_tag.h>
#include <zaf/property/setting.h>
#include "control/common.h"
#include "control/control_operate_panels_creator.h"

static std::shared_ptr<zaf::Control> CreateItemHeightOperatePanel(const std::shared_ptr<zaf::ListBox>& list_box);

void ListBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

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

    CreateControlOperatePanels(list_box, operate_panels);
    operate_panels.push_back(CreateItemHeightOperatePanel(list_box));
}


static std::shared_ptr<zaf::Control> CreateItemHeightOperatePanel(const std::shared_ptr<zaf::ListBox>& list_box) {

    struct Context {
        std::shared_ptr<zaf::TextBox> text_box;
    };

    auto context = std::make_shared<Context>();

    return zaf::SetProperties(
        CreateOperateContainerPanel(1),
        zaf::child = zaf::CreateWithProperties<zaf::Label>(
            zaf::text = L"Item height",
            zaf::maximumWidth = OperatePanelLabelMaximumWidth
        ),
        zaf::child = context->text_box = zaf::CreateWithProperties<zaf::TextBox>(
            zaf::text = zaf::ToWideString(list_box->GetItemHeight(), zaf::ToStringOptions().Precision(2)),
            zaf::textValidator = zaf::GetNumberTextValidator()
        ),
        zaf::child = zaf::CreateWithProperties<zaf::Button>(
            zaf::text = L"Set",
            zaf::onClick = std::bind([context, list_box]() {
                float item_height = zaf::ToNumeric<float>(context->text_box->GetText());
                list_box->SetItemHeight(item_height);
            })
        )
    );
}