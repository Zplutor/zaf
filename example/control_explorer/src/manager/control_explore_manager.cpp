#include "manager/control_explore_manager.h"
#include "property/check_box_property_item.h"
#include "property/color_property_item.h"
#include "property/frame_property_item.h"
#include "property/number_property_item.h"

void ControlExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    AddOptionItems(items);
    AddSizeItems(items);
    AddBorderItems(items);
    AddPaddingItems(items);

    items.insert(items.end(), {
        CreateBackgroundColorPropertyItem(),
        CreateBorderColorPropertyItem(),
    });
}


void ControlExploreManager::AddOptionItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto target_control = GetExploredControl();

    auto is_visible_item = CreateCheckBoxPropertyItem(
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

    auto is_enabled_item = CreateCheckBoxPropertyItem(
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

    auto can_focus_item = CreateCheckBoxPropertyItem(
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


void ControlExploreManager::AddSizeItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto target_control = GetExploredControl();

    auto width_item = CreateFloatPropertyItem(
        L"Width",
        [target_control]() { return target_control->GetWidth(); }, 
        [target_control](float value) {
            target_control->SetWidth(value);
        },
        [target_control](const std::function<void()>& callback) {
            target_control->Subscriptions() +=
                target_control->RectChangeEvent().Subscribe(std::bind(callback));
        }
    );
    items.push_back(width_item);
    
    auto height_item = CreateFloatPropertyItem(
        L"Height",
        [target_control]() { return target_control->GetHeight(); },
        [target_control](float value) {
            target_control->SetHeight(value);
        },
        [target_control](const std::function<void()>& callback) {
            target_control->Subscriptions() +=
                target_control->RectChangeEvent().Subscribe(std::bind(callback));
        }
    );
    items.push_back(height_item);

    auto min_width_item = CreateFloatPropertyItem(
        L"Minimum Width",
        [target_control]() { return target_control->GetMinWidth(); },
        [target_control](float value) {
            target_control->SetMinWidth(value);
        }
    );
    items.push_back(min_width_item);

    auto max_width_item = CreateFloatPropertyItem(
        L"Maximum Width",
        [target_control]() { return target_control->GetMaxWidth(); },
        [target_control](float value) {
            target_control->SetMaxWidth(value);
        }
    );
    items.push_back(max_width_item);

    auto min_height_item = CreateFloatPropertyItem(
        L"Minimum Height",
        [target_control]() { return target_control->GetMinHeight(); },
        [target_control](float value) {
            target_control->SetMinHeight(value);
        }
    );
    items.push_back(min_height_item);

    auto max_height_item = CreateFloatPropertyItem(
        L"Maximum Height",
        [target_control]() { return target_control->GetMaxHeight(); },
        [target_control](float value) {
            target_control->SetMaxHeight(value);
        }
    );
    items.push_back(max_height_item);
}


void ControlExploreManager::AddBorderItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto control = GetTargetControl();
    auto border_item = CreateFramePropertyItem(
        L"Border",
        [control]() { return control->GetBorder(); },
        [control](const zaf::Frame& frame) { control->SetBorder(frame); });

    items.push_back(border_item);
}


void ControlExploreManager::AddPaddingItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    auto control = GetTargetControl();
    auto border_item = CreateFramePropertyItem(
        L"Padding",
        [control]() { return control->GetPadding(); },
        [control](const zaf::Frame& frame) { control->SetPadding(frame); });

    items.push_back(border_item);
}


std::shared_ptr<PropertyItem> ControlExploreManager::CreateBackgroundColorPropertyItem() {

    auto control = GetTargetControl();
    return CreateColorPropertyItem(
        L"Background color",
        [control]() { return control->GetBackgroundColor(); },
        [control](const zaf::Color& color) { control->SetBackgroundColor(color); });
}


std::shared_ptr<PropertyItem> ControlExploreManager::CreateBorderColorPropertyItem() {

    auto control = GetTargetControl();
    return CreateColorPropertyItem(
        L"Border color",
        [control]() { return control->GetBorderColor(); },
        [control](const zaf::Color& color) { control->SetBorderColor(color); });
}