#include <zaf/control/radio_button.h>
#include <algorithm>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/paint_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(RadioButton);


RadioButton::RadioButton() : is_checked_(true) {

}


RadioButton::~RadioButton() {

}


void RadioButton::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());

    SetRadioBorderColorPicker(ColorPicker([](const Control& control) {
        return internal::GetBoxBorderColor(As<ClickableControl>(control));
    }));
    
    SetRadioBackColorPicker(ColorPicker([](const Control& control) {
        return internal::GetBoxBackgroundColor(As<ClickableControl>(control));
    }));
}


zaf::Size RadioButton::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto extra_icon_width = internal::IconSize + internal::IconMargin;

    auto revised_max_size = max_size;
    revised_max_size.width -= extra_icon_width;

    auto result = __super::CalculatePreferredContentSize(revised_max_size);
    result.width += extra_icon_width;
    return result;
}


void RadioButton::UpdateStyle() {

    __super::UpdateStyle();

    radio_back_color_field_.UpdateColor(*this);
    radio_border_color_field_.UpdateColor(*this);
}


void RadioButton::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    auto state_guard = canvas.PushState();

    auto text_layout = GetTextLayout();

    internal::DrawTextWithIcon(
        canvas,
        *this,
        text_layout,
        std::bind(&RadioButton::PaintRadio, this, std::placeholders::_1, std::placeholders::_2));
}


void RadioButton::PaintRadio(Canvas& canvas, const zaf::Rect& radio_rect) const {

    float ellipse_x_radius = radio_rect.size.width / 2;
    float ellipse_y_radius = radio_rect.size.height / 2;

    Point ellipse_position = radio_rect.position;
    ellipse_position.x += ellipse_x_radius;
    ellipse_position.y += ellipse_y_radius;

    Ellipse ellipse(ellipse_position, ellipse_x_radius, ellipse_y_radius);

    canvas.SetBrushWithColor(RadioBackColor());
    canvas.DrawEllipse(ellipse);

    canvas.SetBrushWithColor(RadioBorderColor());
    canvas.DrawEllipseFrame(ellipse, 1);
    
    if (IsChecked()) {
        ellipse.Inflate(-3);
        canvas.DrawEllipse(ellipse);
    }
}


zaf::Rect RadioButton::DetermineTextRect() {
    return zaf::Rect();
}


Color RadioButton::RadioBorderColor() const {
    return radio_border_color_field_.Color();
}


void RadioButton::SetRadioBorderColor(const Color& color) {
    radio_border_color_field_.SetColor(color, *this);
}


const ColorPicker& RadioButton::RadioBorderColorPicker() const {
    return radio_border_color_field_.ColorPicker();
}

void RadioButton::SetRadioBorderColorPicker(ColorPicker picker) {
    radio_border_color_field_.SetColorPicker(std::move(picker), *this);
}


Color RadioButton::RadioBackColor() const {
    return radio_back_color_field_.Color();
}

void RadioButton::SetRadioBackColor(const Color& color) {
    radio_back_color_field_.SetColor(color, *this);
}


const ColorPicker& RadioButton::RadioBackColorPicker() const {
    return radio_back_color_field_.ColorPicker();
}

void RadioButton::SetRadioBackColorPicker(ColorPicker picker) {
    radio_back_color_field_.SetColorPicker(std::move(picker), *this);
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