#include "manager/scroll_bar_explore_manager.h"
#include "property/check_box_property_item.h"
#include "property/number_property_item.h"

namespace {

void ChangeScrollBarSize(const std::shared_ptr<zaf::ScrollBar>& scroll_bar) {

    zaf::Size size;
    if (scroll_bar->IsHorizontal()) {
        size.width = 150;
        size.height = 20;
    }
    else {
        size.width = 20;
        size.height = 150;
    }

    scroll_bar->SetSize(size);
}

}

std::shared_ptr<zaf::Control> ScrollBarExploreManager::CreateExploredControl() {
    
    auto scroll_bar = zaf::Create<zaf::ScrollBar>();
    scroll_bar->SetIsHorizontal(true);
    scroll_bar->SetValueRange(0, 100);
    ChangeScrollBarSize(scroll_bar);
    return scroll_bar;
}


void ScrollBarExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateIsHorizontalPropertyItem(),
        CreateArrowLengthPropertyItem(),
        CreateMinimumValuePropertyItem(),
        CreateMaximumValuePropertyItem(),
        CreateValuePropertyItem(),
        CreateSmallChangeValuePropertyItem(),
        CreateLargeChangeValuePropertyItem(),
    });
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateIsHorizontalPropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateCheckBoxPropertyItem(
        L"Is horizontal",
        [scroll_bar]() { return scroll_bar->IsHorizontal(); },
        [scroll_bar](bool is_horizontal) { 
            scroll_bar->SetIsHorizontal(is_horizontal); 
            ChangeScrollBarSize(scroll_bar);
        }, 
        nullptr);
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateArrowLengthPropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateFloatPropertyItem(
        L"Arrow length",
        [scroll_bar]() { return scroll_bar->GetArrowLength(); },
        [scroll_bar](float value) { scroll_bar->SetArrowLength(value); });
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateMinimumValuePropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateIntegerPropertyItem(
        L"Minimum value",
        [scroll_bar]() { return scroll_bar->GetMinValue(); },
        [scroll_bar](std::int64_t value) { scroll_bar->SetMinValue(static_cast<int>(value)); });
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateMaximumValuePropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateIntegerPropertyItem(
        L"Maximum value",
        [scroll_bar]() { return scroll_bar->GetMaxValue(); },
        [scroll_bar](std::int64_t value) { scroll_bar->SetMaxValue(static_cast<int>(value)); });
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateValuePropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateIntegerPropertyItem(
        L"Value",
        [scroll_bar]() { return scroll_bar->GetValue(); },
        [scroll_bar](std::int64_t value) { scroll_bar->SetValue(static_cast<int>(value)); },
        [scroll_bar](const std::function<void()>& callback) {

            scroll_bar->Subscriptions() +=
                scroll_bar->ScrollEvent().Subscribe(std::bind(callback));
        }
    );
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateSmallChangeValuePropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateIntegerPropertyItem(
        L"Small change",
        [scroll_bar]() { return scroll_bar->GetSmallChange(); },
        [scroll_bar](std::int64_t value) { scroll_bar->SetSmallChange(static_cast<int>(value)); });
}


std::shared_ptr<PropertyItem> ScrollBarExploreManager::CreateLargeChangeValuePropertyItem() {

    auto scroll_bar = GetScrollBar();
    return CreateIntegerPropertyItem(
        L"Large change",
        [scroll_bar]() { return scroll_bar->GetLargeChange(); },
        [scroll_bar](std::int64_t value) { scroll_bar->SetLargeChange(static_cast<int>(value)); });
}