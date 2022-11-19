#include <zaf/control/property_grid/internal/item.h>
#include <zaf/base/as.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::property_grid::internal {
namespace {

constexpr std::uint32_t DelimiterLineColor = 0xeeeeee;

}

Item::Item(
    const std::shared_ptr<Data>& data,
    const std::shared_ptr<ValueView>& value_view,
    const std::shared_ptr<SplitDistanceManager>& split_distance_manager)
    :
    data_(data),
    value_view_(value_view),
    split_distance_manager_(split_distance_manager) {

}


void Item::Initialize() {

    __super::Initialize();

    this->SetBorder(Frame{ 0, 0, 0, 1 });
    this->SetBorderColor(Color::FromRGB(DelimiterLineColor));
    this->SetPadding(Frame{ 2, 0, 0, 0});

    InitializeSubControls();

    auto manager = split_distance_manager_.lock();
    if (manager) {

        Subscriptions() += manager->DistanceChangeSubject().GetObservable().Subscribe(
            [this](const SplitDistanceChangeInfo& event_info) {

            if (this != event_info.changing_item.get()) {
                SetSplitDistance(event_info.new_distance); 
            }
        });
    }
}


void Item::InitializeSubControls() {

    InitializeNameLabel();
    InitializeValueView();
    InitializeSplitControl();
}


void Item::InitializeNameLabel() {

    name_label_ = CreateLabel();
    name_label_->SetPadding(Frame{ 0, 0, 4, 0 });

    name_label_->SetText(data_->Property()->GetName());
}


void Item::InitializeValueView() {

    value_view_->SetAccessMethod([this]() {

        if (data_->Property()->CanSet()) {
            return property_grid::AccessMethod::ReadWrite;
        }

        return property_grid::AccessMethod::ReadOnly;
    }());

    value_view_->SetValue(data_->Value());
    value_view_->SetPadding(Frame{ 4, 0, 4, 0 });
}


std::shared_ptr<Label> Item::CreateLabel() const {

    auto result = Create<Label>();
    result->SetParagraphAlignment(ParagraphAlignment::Center);
    result->SetTextTrimming(TextTrimming::Granularity::Character);
    result->SetTextColorPicker([](const Control& control) {
        if (control.IsSelected()) {
            return Color::White();
        }
        else {
            return Color::FromRGB(zaf::internal::ControlNormalTextColorRGB);
        }
    });
    return result;
}


void Item::InitializeSplitControl() {

    split_control_ = Create<SplitControl>();
    split_control_->SetIsHorizontalSplit(false);
    split_control_->SetFirstPane(name_label_);
    split_control_->SetSecondPane(value_view_);

    split_control_->GetSplitBar()->SetSplitterColorPicker([](const Control& control) {
    
        if (control.IsSelected()) {
            return Color::FromRGB(zaf::internal::ControlSelectedColorRGB);
        }
        return Color::FromRGB(DelimiterLineColor);
    }); 

    Subscriptions() += split_control_->SplitBarDistanceChangeEvent().Subscribe(
        [this](const SplitControlSplitBarDistanceChangeInfo& event_info) {

        auto manager = split_distance_manager_.lock();
        if (manager) {

            SplitDistanceChangeInfo event_info;
            event_info.changing_item = As<Item>(shared_from_this());
            event_info.new_distance = 
                this->GetTextRect().Left() + split_control_->GetSplitBarDistance();

            manager->DistanceChangeSubject().GetObserver().OnNext(event_info);
        }
    });

    this->AddChild(split_control_);
}


void Item::SetSplitDistance(float new_distance) {

    auto revised_distance = new_distance - this->GetTextRect().Left();
    split_control_->SetSplitBarDistance(revised_distance);
}


void Item::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto update_guard = split_control_->BeginUpdate();

    split_control_->SetRect(GetTextRect());

    auto manager = split_distance_manager_.lock();
    if (manager) {
        SetSplitDistance(manager->CurrentDistance());
    }
}


}