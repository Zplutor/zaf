#include <zaf/control/property_grid/internal/property_grid_item.h>
#include <zaf/base/as.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {
namespace {

constexpr std::uint32_t DelimiterLineColor = 0xeeeeee;

}

PropertyGridItem::PropertyGridItem(
    const std::shared_ptr<PropertyGridData>& data,
    const std::shared_ptr<property_grid::ValueView>& value_view,
    const std::shared_ptr<PropertyGridSplitDistanceManager>& split_distance_manager)
    :
    data_(data),
    value_view_(value_view),
    split_distance_manager_(split_distance_manager) {

}


void PropertyGridItem::Initialize() {

    __super::Initialize();

    this->SetBorder(Frame{ 0, 0, 0, 1 });
    this->SetBorderColor(Color::FromRGB(DelimiterLineColor));

    InitializeSubControls();

    auto manager = split_distance_manager_.lock();
    if (manager) {

        Subscriptions() += manager->DistanceChangeSubject().GetObservable().Subscribe(
            [this](const PropertyGridSplitDistanceChangeInfo& event_info) {

            if (this != event_info.changing_item.get()) {
                SetSplitDistance(event_info.new_distance); 
            }
        });
    }
}


void PropertyGridItem::InitializeSubControls() {

    InitializeNameLabel();
    InitializeValueView();
    InitializeSplitControl();
}


void PropertyGridItem::InitializeNameLabel() {

    name_label_ = CreateLabel();
    name_label_->SetPadding(Frame{ 0, 0, 4, 0 });

    name_label_->SetText(data_->Property()->GetName());
}


void PropertyGridItem::InitializeValueView() {

    value_view_->SetPadding(Frame{ 4, 0, 4, 0 });
    value_view_->SetValue(data_->Value());
}


std::shared_ptr<Label> PropertyGridItem::CreateLabel() const {

    auto result = Create<Label>();
    result->SetParagraphAlignment(ParagraphAlignment::Center);
    result->SetTextTrimming(TextTrimming::Granularity::Character);
    result->SetTextColorPicker([](const Control& control) {
        if (control.IsSelected()) {
            return Color::White();
        }
        else {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
    });
    return result;
}


void PropertyGridItem::InitializeSplitControl() {

    split_control_ = Create<SplitControl>();
    split_control_->SetIsHorizontalSplit(false);
    split_control_->GetSplitBar()->SetSplitterColor(Color::FromRGB(DelimiterLineColor));
    split_control_->SetFirstPane(name_label_);
    split_control_->SetSecondPane(value_view_);

    Subscriptions() += split_control_->SplitBarDistanceChangeEvent().Subscribe(
        [this](const SplitControlSplitBarDistanceChangeInfo& event_info) {

        auto manager = split_distance_manager_.lock();
        if (manager) {

            PropertyGridSplitDistanceChangeInfo event_info;
            event_info.changing_item = As<PropertyGridItem>(shared_from_this());
            event_info.new_distance = 
                this->GetItemContentRect().Left() + split_control_->GetSplitBarDistance();

            manager->DistanceChangeSubject().GetObserver().OnNext(event_info);
        }
    });

    this->AddChild(split_control_);
}


void PropertyGridItem::SetSplitDistance(float new_distance) {

    auto revised_distance = new_distance - this->GetItemContentRect().Left();
    split_control_->SetSplitBarDistance(revised_distance);
}


void PropertyGridItem::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto update_guard = split_control_->BeginUpdate();

    split_control_->SetRect(GetItemContentRect());

    auto manager = split_distance_manager_.lock();
    if (manager) {
        SetSplitDistance(manager->CurrentDistance());
    }
}


}