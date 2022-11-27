#include "manager/split_control_explore_manager.h"
#include <zaf/control/label.h>
#include "property/check_box_property_item.h"
#include "property/number_property_item.h"

std::shared_ptr<zaf::Control> SplitControlExploreManager::CreateExploredControl() {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSize(zaf::Size(190, 190));
    split_control->SetBorder(zaf::Frame(1));
    split_control->SetBorderColor(zaf::Color::Black());

    auto first_label = zaf::Create<zaf::Label>();
    first_label->SetBackgroundColor(zaf::Color::White());
    first_label->SetText(L"First pane");
    first_label->SetTextAlignment(zaf::TextAlignment::Center);
    first_label->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    split_control->SetFirstPane(first_label);

    auto second_label = zaf::Create<zaf::Label>();
    second_label->SetBackgroundColor(zaf::Color::White());
    second_label->SetText(L"Second pane");
    second_label->SetTextAlignment(zaf::TextAlignment::Center);
    second_label->SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    split_control->SetSecondPane(second_label);

    return split_control;
}


void SplitControlExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateSplitBarThicknessPropertyItem(),
        CreateIsHorizontalSplitPropertyItem(),
        CreateSplitBarDistancePropertyItem(),
    });
}


std::shared_ptr<PropertyItem> SplitControlExploreManager::CreateSplitBarThicknessPropertyItem() {

    auto split_control = GetSplitControl();
    return CreateFloatPropertyItem(
        L"Split bar thickness",
        [split_control]() { return split_control->SplitBarThickness(); },
        [split_control](float value) { split_control->SetSplitBarThickness(value); });
}


std::shared_ptr<PropertyItem> SplitControlExploreManager::CreateIsHorizontalSplitPropertyItem() {

    auto split_control = GetSplitControl();
    return CreateCheckBoxPropertyItem(
        L"Is horizontal split",
        [split_control]() { return split_control->IsHorizontalSplit(); },
        [split_control](bool value) { split_control->SetIsHorizontalSplit(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> SplitControlExploreManager::CreateSplitBarDistancePropertyItem() {

    auto split_control = GetSplitControl();
    return CreateFloatPropertyItem(
        L"Split bar distance",
        [split_control]() { return split_control->SplitDistance(); },
        [split_control](float value) { split_control->SetSplitDistance(value); },
        [split_control](const std::function<void()>& callback) {

            split_control->Subscriptions() +=
                split_control->SplitBarDistanceChangeEvent().Subscribe(std::bind(callback));
        });
}


