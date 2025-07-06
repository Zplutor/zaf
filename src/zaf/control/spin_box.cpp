#include <zaf/control/spin_box.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/control_object.h>
#include <zaf/control/spin_button.h>
#include <zaf/control/spin_edit.h>
#include <zaf/creation.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>

namespace zaf {

ZAF_OBJECT_IMPL(SpinBox);


void SpinBox::Initialize() {

    __super::Initialize();

    auto update_guard = this->BeginUpdate();

    this->SetBorder(zaf::Frame{ 1 });
    this->SetBorderColor(zaf::Color::Black());
    this->SetBackgroundColor(zaf::Color::White());

    edit_ = Create<SpinEdit>();
    InitializeEdit();

    up_button_ = Create<SpinButton>();
    InitializeUpButton();

    down_button_ = Create<SpinButton>();
    InitializeDownButton();
}


void SpinBox::InitializeEdit() {

    edit_subscriptions_ += edit_->TextChangedEvent().Subscribe(std::bind([this]() {
    
        if (!is_setting_value_) {
            if (edit_->TextLength() > 0) {
                CommitEditChange();
            }
        }

        //Always keep the caret at the end of text.
        edit_->SetSelectionRange(Range{ edit_->TextLength(), 0 });
    }));

    edit_subscriptions_ += edit_->FocusLostEvent().Subscribe(std::bind([this]() {
        CommitEditChange();
    }));

    edit_subscriptions_ += edit_->KeyDownEvent().Subscribe([this](const KeyDownInfo& event_info) {
    
        if (event_info.Message().Key() == Key::Enter) {
            CommitEditChange();
            event_info.MarkAsHandled();
        }
    });

    this->AddChild(edit_);
}


void SpinBox::CommitEditChange() {

    auto text_value = ToNumeric<int>(edit_->Text());
    SetValue(text_value);

    if (text_value != value_) {
        auto auto_reset = MakeAutoReset(is_setting_value_, true);
        edit_->SetText(std::to_wstring(value_));
    }
}


void SpinBox::InitializeUpButton() {

    up_button_->SetDirection(ArrowDirection::Up);

    InitializeButtonEvents(
        true,
        up_button_,
        up_button_subscriptions_,
        up_button_timer_subscription_);

    this->AddChild(up_button_);
}


void SpinBox::InitializeDownButton() {

    down_button_->SetDirection(ArrowDirection::Down);

    InitializeButtonEvents(
        false,
        down_button_,
        down_button_subscriptions_, 
        down_button_timer_subscription_);

    this->AddChild(down_button_);
}


void SpinBox::InitializeButtonEvents(
    bool increase,
    const std::shared_ptr<SpinButton>& button,
    SubscriptionBag& subscriptions,
    Subscription& timer_subscription) {

    subscriptions += button->MouseDownEvent().Subscribe(
        [this, increase, &timer_subscription](const zaf::MouseDownInfo& event_info) {
    
        ChangeValueByButtonEvent(increase);

        auto timer = rx::Timer(
            std::chrono::milliseconds(300),
            std::chrono::milliseconds(50),
            Scheduler::Main());

        timer_subscription = timer.Subscribe(std::bind([this, increase]() {
            ChangeValueByButtonEvent(increase);
        }));

        event_info.MarkAsHandled();
    });

    subscriptions += button->MouseUpEvent().Subscribe(
        [this, &timer_subscription](const zaf::MouseUpInfo& event_info) {
    
        timer_subscription.Unsubscribe();
    });
}


void SpinBox::ChangeValueByButtonEvent(bool increase) {

    auto new_value = Value();
    if (increase) {
        ++new_value;
    }
    else {
        --new_value;
    }
    SetValue(new_value);
    
    edit_->SetIsFocused(true);
}


void SpinBox::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto content_size = ContentSize();

    zaf::Rect edit_rect{ 0, 0, content_size.width - button_width_, content_size.height };
    edit_->SetRect(edit_rect);

    zaf::Rect up_button_rect{ edit_rect.Right(), 0, button_width_, content_size.height / 2 };
    up_button_->SetRect(up_button_rect);

    down_button_->SetRect(zaf::Rect{ 
        edit_rect.Right(), 
        up_button_rect.Bottom(),
        button_width_,
        content_size.height / 2 
    });
}


int SpinBox::Value() const {
    return value_;
}

void SpinBox::SetValue(int value) {

    int revised_value = (std::max)(value, min_value_);
    revised_value = (std::min)(revised_value, max_value_);
    if (revised_value == value_) {
        return;
    }

    int old_value = value_;
    value_ = revised_value;

    {
        auto auto_reset = MakeAutoReset(is_setting_value_, true);
        edit_->SetText(std::to_wstring(value_));
    }

    OnValueChanged(SpinBoxValueChangedInfo{ As<SpinBox>(shared_from_this()), old_value });
}


Observable<SpinBoxValueChangedInfo> SpinBox::ValueChangedEvent() const {
    return value_changed_event_.GetObservable();
}

void SpinBox::OnValueChanged(const SpinBoxValueChangedInfo& event_info) {
    value_changed_event_.Raise(event_info);
}


int SpinBox::MinValue() const {
    return min_value_;
}

void SpinBox::SetMinValue(int min_value) {

    min_value_ = min_value;

    if (max_value_ < min_value_) {
        SetMaxValue(min_value_);
    }

    if (value_ < min_value_) {
        SetValue(min_value_);
    }
}


int SpinBox::MaxValue() const {
    return max_value_;
}

void SpinBox::SetMaxValue(int max_value) {

    max_value_ = max_value;

    if (min_value_ > max_value_) {
        SetMinValue(max_value_);
    }

    if (value_ > max_value_) {
        SetValue(max_value_);
    }
}

}