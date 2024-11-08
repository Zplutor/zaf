#include <zaf/internal/property_grid/property_grid_item.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/control/list_item.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {
namespace {

constexpr std::uint32_t DelimiterLineColor = 0xeeeeee;

}

PropertyGridItem::PropertyGridItem(
    std::shared_ptr<PropertyData> property_data,
    std::shared_ptr<property_grid::ValueEditor> value_editor,
    std::shared_ptr<SplitDistanceManager> split_distance_manager)
    :
    property_data_(std::move(property_data)),
    value_editor_(std::move(value_editor)),
    split_distance_manager_(std::move(split_distance_manager)) {

}


void PropertyGridItem::Initialize() {

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


void PropertyGridItem::InitializeSubControls() {

    InitializeNameLabel();
    InitializeValueView();
    InitializeSplitControl();
}


void PropertyGridItem::InitializeNameLabel() {

    name_label_ = CreateLabel();
    name_label_->SetPadding(Frame{ 0, 0, 4, 0 });

    name_label_->SetText(std::wstring{ property_data_->Property()->Name() });
}


void PropertyGridItem::InitializeValueView() {

    value_editor_->SetAccessMethod([this]() {

        if (property_data_->IsReadOnly()) {
            return property_grid::AccessMethod::ReadOnly;
        }
        return property_grid::AccessMethod::ReadWrite;
    }());

    value_editor_->SetValue(property_data_->Value());
    value_editor_->SetPadding(Frame{ 4, 0, 4, 0 });

    Subscriptions() += value_editor_->ValueChangedEvent().Subscribe(
        [this](const std::shared_ptr<Object>& new_value) {
    
        if (!new_value->IsEqual(*property_data_->Value())) {
            property_data_->ChangeValueFromDownToUp(new_value);
        }
    });
}


std::shared_ptr<Label> PropertyGridItem::CreateLabel() {

    auto result = Create<Label>();
    result->SetParagraphAlignment(dwrite::ParagraphAlignment::Center);
    result->SetTextTrimming(dwrite::TextTrimmingGranularity::Character);
    result->SetTextColorPicker(ColorPicker([this](const Control& control) {
    
        if (control.IsSelectedInContext()) {
            return Color::White();
        }

        if (property_data_->IsReadOnly()) {
            return Color::FromRGB(zaf::internal::ControlDisabledTextColorRGB);
        }

        return Color::FromRGB(zaf::internal::ControlNormalTextColorRGB);
    }));

    return result;
}


void PropertyGridItem::InitializeSplitControl() {

    split_control_ = Create<SplitControl>();
    split_control_->SetIsHorizontalSplit(false);
    split_control_->SetFirstPane(name_label_);
    split_control_->SetSecondPane(value_editor_);

    split_control_->SplitBar()->SetSplitterColorPicker(ColorPicker([](const Control& control) {

        if (control.IsSelectedInContext()) {

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
    }));

    Subscriptions() += split_control_->SplitDistanceChangedEvent().Subscribe(
        [this](const SplitDistanceChangedInfo& event_info) {

        //Don't raise event again if it is handling distance changed event.
        if (is_handling_split_distance_event_) {
            return;
        }

        auto manager = split_distance_manager_.lock();
        if (manager) {

            ItemSplitDistanceChangedInfo new_event_info;
            new_event_info.changing_item = As<PropertyGridItem>(shared_from_this());
            new_event_info.new_distance =
                this->DetermineTextRect().Left() + split_control_->SplitDistance();
            new_event_info.is_changed_by_dragging = event_info.IsChangedByDragging();

            manager->DistanceChangedSubject().AsObserver().OnNext(new_event_info);
        }
    });

    this->AddChild(split_control_);
}


void PropertyGridItem::SetAbsoluteSplitDistance(float new_distance) {

    auto revised_distance = new_distance - this->DetermineTextRect().Left();
    split_control_->SetSplitDistance(revised_distance);
}


void PropertyGridItem::SetFirstPaneMinLength(float max_x) {

    auto difference = max_x - split_control_->X();
    if (difference < 0) {
        return;
    }

    split_control_->SetFirstPaneMinLength(difference);
}


void PropertyGridItem::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto update_guard = split_control_->BeginUpdate();

    split_control_->SetRect(DetermineTextRect());

    auto manager = split_distance_manager_.lock();
    if (manager) {

        SetFirstPaneMinLength(manager->MaxSplitControlX());
        SetAbsoluteSplitDistance(manager->CurrentDistance());
    }
}


void PropertyGridItem::OnParentChanged(const ParentChangedInfo& event_info) {

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

}