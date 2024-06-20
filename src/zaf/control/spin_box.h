#pragma once

#include <zaf/base/event/event.h>
#include <zaf/control/control.h>
#include <zaf/control/event/spin_box_value_changed_info.h>

namespace zaf {

class SpinButton;
class SpinEdit;

class SpinBox : public zaf::Control {
public:
    ZAF_OBJECT;

    int Value() const;
    void SetValue(int value);
    Observable<SpinBoxValueChangedInfo> ValueChangedEvent() const;

    int MinValue() const;
    void SetMinValue(int min_value);

    int MaxValue() const;
    void SetMaxValue(int max_value);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

    virtual void OnValueChanged(const SpinBoxValueChangedInfo& event_info);

private:
    void InitializeEdit();
    void CommitEditChange();
    void InitializeUpButton();
    void InitializeDownButton();
    void InitializeButtonEvents(
        bool is_down_button,
        const std::shared_ptr<SpinButton>& button, 
        SubscriptionSet& subscriptions,
        Subscription& timer_subscription);
    void ChangeValueByButtonEvent(bool increase);

private:
    std::shared_ptr<SpinEdit> edit_;
    SubscriptionSet edit_subscriptions_;
    bool is_setting_value_{};

    std::shared_ptr<SpinButton> up_button_;
    SubscriptionSet up_button_subscriptions_;
    Subscription up_button_timer_subscription_;

    std::shared_ptr<SpinButton> down_button_;
    SubscriptionSet down_button_subscriptions_;
    Subscription down_button_timer_subscription_;

    float button_width_{ 16 };

    int value_{ 0 };
    int min_value_{ 0 };
    int max_value_{ 0 };

    Event<SpinBoxValueChangedInfo> value_changed_event_;
};

ZAF_OBJECT_BEGIN(SpinBox);
ZAF_PROPERTY(MaxValue);
ZAF_PROPERTY(MinValue);
ZAF_PROPERTY(Value);
ZAF_OBJECT_END;

}