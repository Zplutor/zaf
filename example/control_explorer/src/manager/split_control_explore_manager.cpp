#include "manager/split_control_explore_manager.h"
#include <zaf/control/label.h>

std::shared_ptr<zaf::Control> SplitControlExploreManager::CreateExploredControl() {

    auto split_control = zaf::Create<zaf::SplitControl>();
    split_control->SetSize(zaf::Size(190, 190));
    split_control->SetBorder(zaf::Frame(1));
    split_control->SetBorderColor(zaf::Color::Black());

    auto first_label = zaf::Create<zaf::Label>();
    first_label->SetBackgroundColor(zaf::Color::White());
    first_label->SetText(L"First pane");
    first_label->SetTextAlignment(zaf::dwrite::TextAlignment::Center);
    first_label->SetParagraphAlignment(zaf::dwrite::ParagraphAlignment::Center);
    split_control->SetFirstPane(first_label);

    auto second_label = zaf::Create<zaf::Label>();
    second_label->SetBackgroundColor(zaf::Color::White());
    second_label->SetText(L"Second pane");
    second_label->SetTextAlignment(zaf::dwrite::TextAlignment::Center);
    second_label->SetParagraphAlignment(zaf::dwrite::ParagraphAlignment::Center);
    split_control->SetSecondPane(second_label);

    return split_control;
}
