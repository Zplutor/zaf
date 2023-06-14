#include <zaf/control/radio_button.h>
#include <algorithm>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/paint_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_DEFINE_TYPE(RadioButton)
ZAF_DEFINE_TYPE_PROPERTY(AutoCheck)
ZAF_DEFINE_TYPE_PROPERTY(IsChecked)
ZAF_DEFINE_TYPE_PROPERTY(RadioBackgroundColor)
ZAF_DEFINE_TYPE_PROPERTY(RadioBorderColor)
ZAF_DEFINE_TYPE_END


RadioButton::RadioButton() : is_checked_(true) {

}


RadioButton::~RadioButton() {

}


void RadioButton::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([](const Control&) {
        return Color::Transparent();
    });
}


zaf::Size RadioButton::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto extra_icon_width = internal::IconSize + internal::IconMargin;

    auto revised_max_size = max_size;
    revised_max_size.width -= extra_icon_width;

    auto result = __super::CalculatePreferredContentSize(revised_max_size);
    result.width += extra_icon_width;
    return result;
}


void RadioButton::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto state_guard = canvas.PushState();

    auto text_layout = GetTextLayout();
    if (!text_layout) {
        return;
    }

    internal::DrawTextWithIcon(
        canvas,
        *this,
        text_layout,
        std::bind(&RadioButton::PaintRadio, this, std::placeholders::_1, std::placeholders::_2));
}


void RadioButton::PaintRadio(Canvas& canvas, const zaf::Rect& radio_rect) {

    float ellipse_x_radius = radio_rect.size.width / 2;
    float ellipse_y_radius = radio_rect.size.height / 2;

    Point ellipse_position = radio_rect.position;
    ellipse_position.x += ellipse_x_radius;
    ellipse_position.y += ellipse_y_radius;

    Ellipse ellipse(ellipse_position, ellipse_x_radius, ellipse_y_radius);

    canvas.SetBrushWithColor(RadioBackgroundColor());
    canvas.DrawEllipse(ellipse);

    canvas.SetBrushWithColor(RadioBorderColor());
    canvas.DrawEllipseFrame(ellipse, 1);
    
    if (IsChecked()) {
        ellipse.Inflate(-3);
        canvas.DrawEllipse(ellipse);
    }
}


zaf::Rect RadioButton::GetTextRect() {
    return zaf::Rect();
}


ColorPicker RadioButton::RadioBorderColorPicker() const {

    if (radio_border_color_picker_) {
        return radio_border_color_picker_;
    }

    return internal::GetBoxBorderColorPicker();
}


void RadioButton::SetRadioBorderColorPicker(const ColorPicker& color_picker) {

    radio_border_color_picker_ = color_picker;
    NeedRepaint();
}


ColorPicker RadioButton::RadioBackgroundColorPicker() const {

    if (radio_background_color_picker_) {
        return radio_background_color_picker_;
    }

    return internal::GetBoxBackgroundColorPicker();
}


void RadioButton::SetRadioBackgroundColorPicker(const ColorPicker& color_picker) {

    radio_background_color_picker_ = color_picker;
    NeedRepaint();
}


void RadioButton::SetAssociatedGroup(const std::shared_ptr<Group>& group) {

    if (group_ == group) {
        return;
    }

    auto shared_this = std::dynamic_pointer_cast<RadioButton>(shared_from_this());

    if (group_ != nullptr) {
        group_->Remove(shared_this);
    }

    group_ = group;
    if (group_ != nullptr) {
        group_->Add(shared_this);
    }
}


bool RadioButton::AutoCheck() const {
    return auto_check_;
}


void RadioButton::SetAutoCheck(bool auto_check) {
    auto_check_ = auto_check;
}


void RadioButton::ChangeCheckState(bool is_checked) {

    if (is_checked == IsChecked()) {
        return;
    }

    is_checked_ = is_checked;
    NeedRepaint();

    auto shared_this = std::dynamic_pointer_cast<RadioButton>(shared_from_this());

    if (is_checked && group_ != nullptr) {
        group_->OnRadioButtonChecked(shared_this);
    }

    OnCheckStateChanged(CheckStateChangedInfo{ As<RadioButton>(shared_from_this()) });
}


void RadioButton::OnCheckStateChanged(const CheckStateChangedInfo& event_info) {
    check_state_changed_event_.Raise(event_info);
}


Observable<CheckStateChangedInfo> RadioButton::CheckStateChangedEvent() const {
    return check_state_changed_event_.GetObservable();
}


void RadioButton::OnClick(const ClickInfo& event_info) {

    __super::OnClick(event_info);

    if (AutoCheck()) {
        SetIsChecked(true);
    }
}


const std::vector<std::shared_ptr<RadioButton>> RadioButton::Group::RadioButtons() const {

    std::vector<std::shared_ptr<RadioButton>> radio_buttons;

    for (const auto& each_radio_button : radio_buttons_) {

        auto radio_button = each_radio_button.lock();
        if (radio_button != nullptr) {
            radio_buttons.push_back(radio_button);
        }
    }

    return radio_buttons;
}


void RadioButton::Group::Add(const std::shared_ptr<RadioButton>& radio_button) {

    radio_buttons_.push_back(radio_button);

    if (radio_buttons_.size() == 1) {
        radio_button->SetIsChecked(true);
    }
    else {
        radio_button->Uncheck();
    }
}


void RadioButton::Group::Remove(const std::shared_ptr<RadioButton>& radio_button) {

    auto iterator = std::remove_if(
        radio_buttons_.begin(), 
        radio_buttons_.end(), 
        [&radio_button](const std::weak_ptr<RadioButton>& each_radio_button) {
            return each_radio_button.lock() == radio_button;
        }
    );

    radio_buttons_.erase(iterator, radio_buttons_.end());

    if (radio_button->IsChecked() && ! radio_buttons_.empty()) {

        auto first_radio_button = radio_buttons_.front().lock();
        first_radio_button->SetIsChecked(true);
    }
}


void RadioButton::Group::OnRadioButtonChecked(
    const std::shared_ptr<RadioButton>& checked_radio_button) {

    for (const auto& each_radio_button : radio_buttons_) {

        auto radio_button = each_radio_button.lock();
        if (radio_button != checked_radio_button) {
            radio_button->Uncheck();
        }
    }
}

}