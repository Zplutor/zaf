#include "operate/control_operate_item_factory.h"
#include <zaf/creation.h>
#include "operate_item_creation.h"

static std::shared_ptr<zaf::Control> CreateFramePanel(
    const std::wstring& title,
    const std::function<zaf::Frame()>& get_frame, 
    const std::function<void(const zaf::Frame&)>& frame_change);

void ControlOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    AddOptionItems(items);
    AddSizeItems(items);
    items.push_back(CreateBorderItems());
    items.push_back(CreatePaddingItems());
}


void ControlOperateItemFactory::AddOptionItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    const auto& target_control = GetTargetControl();

    auto is_visible_item = CreateOperateItemWithCheckBox(
        L"Is visible",
        [target_control]() { 
            return target_control->IsVisible(); 
        },
        [target_control](bool value) {
            target_control->SetIsVisible(value);
        },
        nullptr
    );
    items.push_back(is_visible_item);

    auto is_enabled_item = CreateOperateItemWithCheckBox(
        L"Is enabled",
        [target_control]() { 
            return target_control->IsEnabled(); 
        },
        [target_control](bool value) {
            target_control->SetIsEnabled(value);
        },
        nullptr
    );
    items.push_back(is_enabled_item);

    auto can_focus_item = CreateOperateItemWithCheckBox(
        L"Can be focused",
        [target_control]() { 
            return target_control->CanFocused(); 
        },
        [target_control](bool value) {
            target_control->SetCanFocused(value);
        },
        nullptr
    );
    items.push_back(can_focus_item);
}


void ControlOperateItemFactory::AddSizeItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    const auto& target_control = GetTargetControl();

    auto width_item = CreateOperateItemWithNumericTextBox(
        L"Width",
        [target_control]() { return target_control->GetWidth(); }, 
        [target_control](float value) {
            target_control->SetWidth(value);
        }
    );
    items.push_back(width_item);
    
    auto height_item = CreateOperateItemWithNumericTextBox(
        L"Height",
        [target_control]() { return target_control->GetHeight(); },
        [target_control](float value) {
            target_control->SetHeight(value);
        }
    );
    items.push_back(height_item);

    auto min_width_item = CreateOperateItemWithNumericTextBox(
        L"Minimum Width",
        [target_control]() { return target_control->GetMinimumWidth(); },
        [target_control](float value) {
            target_control->SetMinimumWidth(value);
        }
    );
    items.push_back(min_width_item);

    auto max_width_item = CreateOperateItemWithNumericTextBox(
        L"Maximum Width",
        [target_control]() { return target_control->GetMaximumWidth(); },
        [target_control](float value) {
            target_control->SetMaximumWidth(value);
        }
    );
    items.push_back(max_width_item);

    auto min_height_item = CreateOperateItemWithNumericTextBox(
        L"Minimum Height",
        [target_control]() { return target_control->GetMinimumHeight(); },
        [target_control](float value) {
            target_control->SetMinimumHeight(value);
        }
    );
    items.push_back(min_height_item);

    auto max_height_item = CreateOperateItemWithNumericTextBox(
        L"Maximum Height",
        [target_control]() { return target_control->GetMaximumHeight(); },
        [target_control](float value) {
            target_control->SetMaximumHeight(value);
        }
    );
    items.push_back(max_height_item);
}


std::shared_ptr<zaf::Control> ControlOperateItemFactory::CreateBorderItems() {

    const auto& target_control = GetTargetControl();

    auto border_panel = CreateFramePanel(
        L"Border", 
        [target_control]() { return target_control->GetBorder(); }, 
        [target_control](const zaf::Frame& frame) { 
            target_control->SetBorder(frame);
        }
    );

    return border_panel;
}


std::shared_ptr<zaf::Control> ControlOperateItemFactory::CreatePaddingItems() {

    const auto& target_control = GetTargetControl();

    auto padding_panel = CreateFramePanel(
        L"Padding",
        [target_control]() { return target_control->GetPadding(); },
        [target_control](const zaf::Frame& frame) {
            target_control->SetPadding(frame);
        }
    );

    return padding_panel;
}


static std::shared_ptr<zaf::Control> CreateFramePanel(
    const std::wstring& title,
    const std::function<zaf::Frame()>& get_frame,
    const std::function<void(const zaf::Frame&)>& frame_change) {

    auto title_panel = zaf::Create<OperateItem>();
    title_panel->SetName(title);
    
    auto left = CreateOperateItemWithNumericTextBox(
        L"    Left", 
        [get_frame]() { return get_frame().left; }, 
        [get_frame, frame_change](float value) {
            auto new_frame = get_frame();
            new_frame.left = value;
            frame_change(new_frame);
        }
    );

    auto top = CreateOperateItemWithNumericTextBox(
        L"    Top",
        [get_frame]() { return get_frame().top; },
        [get_frame, frame_change](float value) {
            auto new_frame = get_frame();
            new_frame.top = value;
            frame_change(new_frame);
        }
    );

    auto right = CreateOperateItemWithNumericTextBox(
        L"    Right",
        [get_frame]() { return get_frame().right; },
        [get_frame, frame_change](float value) {
            auto new_frame = get_frame();
            new_frame.right = value;
            frame_change(new_frame);
        }
    );

    auto bottom = CreateOperateItemWithNumericTextBox(
        L"    Bottom",
        [get_frame]() { return get_frame().bottom; },
        [get_frame, frame_change](float value) {
            auto new_frame = get_frame();
            new_frame.bottom = value;
            frame_change(new_frame);
        }
    );

    auto panel = zaf::Create<OperateItemGroup>();
    panel->AddOperateItem(title_panel);
    panel->AddOperateItem(left);
    panel->AddOperateItem(top);
    panel->AddOperateItem(right);
    panel->AddOperateItem(bottom);
    return panel;
}


