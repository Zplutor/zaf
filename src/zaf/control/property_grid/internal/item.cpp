#include <zaf/control/property_grid/internal/item.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/control/list_item.h>
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
    if (!manager) {
        return;
    }

    Subscriptions() += manager->DistanceChangedSubject().AsObservable().Subscribe(
        [this](const ItemSplitDistanceChangedInfo& event_info) {

        auto auto_reset = MakeAutoReset(is_handling_split_distance_event_, true);

        if (this != event_info.changing_item.get()) {
            SetAbsoluteSplitDistance(event_info.new_distance); 
        }
    });

    Subscriptions() += manager->MaxSplitControlXChangedEvent().Subscribe([this](float new_x) {

        SetFirstPaneMinLength(new_x);
    });
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

        if (data_->IsReadOnly()) {
            return property_grid::AccessMethod::ReadOnly;
        }
        return property_grid::AccessMethod::ReadWrite;
    }());

    value_view_->SetValue(data_->Value());
    value_view_->SetPadding(Frame{ 4, 0, 4, 0 });

    Subscriptions() += value_view_->ValueChangedEvent().Subscribe(
        [this](const std::shared_ptr<Object>& new_value) {
    
        if (!new_value->IsEqual(*data_->Value())) {
            data_->ChangeValueFromDownToUp(new_value);
        }
    });
}


std::shared_ptr<Label> Item::CreateLabel() {

    auto result = Create<Label>();
    result->SetParagraphAlignment(ParagraphAlignment::Center);
    result->SetTextTrimming(TextTrimmingGranularity::Character);

    Subscriptions() += result->VisualStateUpdateEvent().Subscribe(
        [this](const VisualStateUpdateInfo& event_info) {
    
        auto label = As<Label>(event_info.Source());
        label->SetTextColor([this, &label]() {
        
            if (label->IsSelectedInContext()) {
                return Color::White();
            }

            if (data_->IsReadOnly()) {
                return Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB);
            }

            return Color::FromRGB(zaf::internal::ControlNormalTextColorRGB);
        }());
    });
    return result;
}


void Item::InitializeSplitControl() {

    split_control_ = Create<SplitControl>();
    split_control_->SetIsHorizontalSplit(false);
    split_control_->SetFirstPane(name_label_);
    split_control_->SetSecondPane(value_view_);

    split_control_->SplitBar()->SetSplitterColorPicker([this](const Control& control) {

        if (this->IsSelectedInContext()) {

            auto parent = control.Parent();
            while (parent) {

                auto list_item = As<ListItem>(parent);
                if (list_item) {
                    return list_item->BackgroundColor();
                }

                parent = parent->Parent();
            }
        }

        return Color::FromRGB(DelimiterLineColor);
    }); 

    Subscriptions() += split_control_->SplitDistanceChangedEvent().Subscribe(
        [this](const SplitDistanceChangedInfo& event_info) {

        //Don't raise event again if it is handling distance changed event.
        if (is_handling_split_distance_event_) {
            return;
        }

        auto manager = split_distance_manager_.lock();
        if (manager) {

            ItemSplitDistanceChangedInfo new_event_info;
            new_event_info.changing_item = As<Item>(shared_from_this());
            new_event_info.new_distance =
                this->DetermineTextRect().Left() + split_control_->SplitDistance();
            new_event_info.is_changed_by_dragging = event_info.IsChangedByDragging();

            manager->DistanceChangedSubject().AsObserver().OnNext(new_event_info);
        }
    });

    this->AddChild(split_control_);
}


void Item::SetAbsoluteSplitDistance(float new_distance) {

    auto revised_distance = new_distance - this->DetermineTextRect().Left();
    split_control_->SetSplitDistance(revised_distance);
}


void Item::SetFirstPaneMinLength(float max_x) {

    auto difference = max_x - split_control_->X();
    if (difference < 0) {
        return;
    }

    split_control_->SetFirstPaneMinLength(difference);
}


void Item::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto update_guard = split_control_->BeginUpdate();

    split_control_->SetRect(DetermineTextRect());

    auto manager = split_distance_manager_.lock();
    if (manager) {

        SetFirstPaneMinLength(manager->MaxSplitControlX());
        SetAbsoluteSplitDistance(manager->CurrentDistance());
    }
}


void Item::OnParentChanged(const ParentChangedInfo& event_info) {

    __super::OnParentChanged(event_info);

    auto split_distance_manager = split_distance_manager_.lock();
    if (!split_distance_manager) {
        return;
    }

    auto new_parent = Parent();
    if (new_parent) {
        split_distance_manager->OnSplitControlAdd(split_control_);
    }
    else {
        split_distance_manager->OnSplitControlRemove(split_control_);
    }
}


void Item::RecoverFocus() {
    value_view_->RecoverFocus();
}

}