#pragma once 

#include <zaf/control/clickable_control.h>
#include <zaf/control/event/check_state_changed_info.h>
#include <zaf/rx/observable.h>

namespace zaf {

/**
 Represents a radio button conrol.
 */
class RadioButton : public ClickableControl {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_PROPERTY;

public:
    /**
     Represents a radio button group.

     Radio buttons associate with the same group cannot be checked at the same time.
     Use RadioButton::SetAssociatedGroup method to associate a radio buton with a group.
     */
    class Group {
    public:
        /**
         Get associated radio buttons.
         */
        const std::vector<std::shared_ptr<RadioButton>> RadioButtons() const;

    private:
        friend class RadioButton;

        void Add(const std::shared_ptr<RadioButton>& radio_button);
        void Remove(const std::shared_ptr<RadioButton>& radio_button);

        void OnRadioButtonChecked(const std::shared_ptr<RadioButton>& radio_button);

    private:
        std::vector<std::weak_ptr<RadioButton>> radio_buttons_;
    };

public:
    RadioButton();
    ~RadioButton();

    /**
     Get radio border color.
     */
    Color RadioBorderColor() const;

    /**
     Set radio border color.
     */
    void SetRadioBorderColor(const Color& color);

    /**
     Get radio background color.
     */
    Color RadioBackgroundColor() const;

    /**
     Set radio background color.
     */
    void SetRadioBackgroundColor(const Color& color);

    /**
     Get the associated group.

     @return
        Return nullptr if there is no associated group.
     */
    const std::shared_ptr<Group>& AssociatedGroup() const {
        return group_;
    }

    /**
     Set the associated group.

     The first radio button associates with a group would be checked.
     The later associated radio buttons would be unchecked.

     If group is nullptr, the association would be removed.
     */
    void SetAssociatedGroup(const std::shared_ptr<Group>& group);

    /**
     Get a value indicating whether the radio button changes its check state automatically
     when it is clicked.

     The default value is true.
     */
    bool AutoCheck() const;

    /**
     Set a value indicating whether the radio button changes its check state automatically
     when it is clicked.
     */
    void SetAutoCheck(bool auto_check);

    /**
     Get a value indicating whether the radio button is checked.
     */
    bool IsChecked() const {
        return is_checked_;
    }

    /**
     Sets a value indicating that whether the radio button is checked.

     Note that the purpose of this method is to check a radio button, rather than to uncheck a
     radio button.

     If a radio button is checked, other radio buttons associate with the same group would be
     unchecked automatically.
     */
    void SetIsChecked(bool is_checked) {
        if (is_checked) {
            ChangeCheckState(true);
        }
    }

    Observable<CheckStateChangedInfo> CheckStateChangedEvent() const;

protected:
    void Initialize() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect DetermineTextRect() override;
    void OnClick(const ClickInfo& event_info) override;

    /**
    Handles check state changed notification. 

    The default implementation raises check state changed event. Derived classes should call the 
    same method of base class to raise the event if they override it.
    */
    virtual void OnCheckStateChanged(const CheckStateChangedInfo& event_info);

private:
    void PaintRadio(Canvas& canvas, const zaf::Rect& radio_rect);

    //Called by Group internally.
    void Uncheck() {
        ChangeCheckState(false);
    }

    void ChangeCheckState(bool is_checked);

private:
    bool auto_check_{ true };
    bool is_checked_;
    std::shared_ptr<Group> group_;

    Color radio_background_color_;
    Color radio_border_color_;

    Event<CheckStateChangedInfo> check_state_changed_event_;
};

ZAF_PROPERTY_BEGIN(RadioButton);
ZAF_PROPERTY(AutoCheck)
ZAF_PROPERTY(IsChecked)
ZAF_PROPERTY(RadioBackgroundColor)
ZAF_PROPERTY(RadioBorderColor)
ZAF_PROPERTY_END;

}