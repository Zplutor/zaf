#pragma once

#include <zaf/control/check_state.h>
#include <zaf/control/clickable_control.h>
#include <zaf/control/event/check_state_changed_info.h>
#include <zaf/object/property_support.h>
#include <zaf/rx/observable.h>

namespace zaf {

/**
 Represents a check box control.    
 */
class CheckBox : public ClickableControl {
public:
    ZAF_OBJECT;

public:
    CheckBox();
    ~CheckBox();

    /**
     Get the box border color.
     */
    Color BoxBorderColor() const;

    /**
     Set the box border color.
     */
    void SetBoxBorderColor(const Color& color);

    const ColorPicker& BoxBorderColorPicker() const;
    void SetBoxBorderColorPicker(ColorPicker picker);

    /**
     Get the box background color.
     */
    Color BoxBackColor() const;

    /**
     Set the box background color.
     */
    void SetBoxBackColor(const Color& color);

    const ColorPicker& BoxBackColorPicker() const;
    void SetBoxBackColorPicker(ColorPicker picker);

    /**
     Get a value indicating that whether the check box changes its check state when 
     it is clicked.

     The default value is true.
     */
    bool AutoCheck() const;

    /**
     Set a value indicating that whether the check box changes its check state when
     it is clicked.
     */
    void SetAutoCheck(bool auto_check);

    /**
     Get a value indicating that whether the check box can be in indeterminate state.

     The default value is false.
     */
    bool CanBeIndeterminate() const;

    /**
     Set a value indicating that whether the check box can be in indeterminate state.

     The check state would be changed to checked if this property is set to false while 
     the previous check state is indeterminate.
     */
    void SetCanBeIndeterminate(bool can_be_indeterminate);

    /**
     Get the check state of the check box.

     The default value is CheckState::Unchecked.
     */
    CheckState CheckState() const {
        return check_state_;
    }

    /**
     Set the check state of the check box.
     */
    void SetCheckState(zaf::CheckState check_state);

    /**
     Get a value indicating that whether the check box is checked.

     @return
     Return true if the check box is in either checked or indeterminate state; 
     otherwise return false.

     The default value is false.
     */
    bool IsChecked() const {
        zaf::CheckState check_state = CheckState();
        return (check_state == CheckState::Checked) || (check_state == CheckState::Indeterminate);
    }

    /**
     Set a value indicating that whether the check box is checked.

     If the value being set is the same as the return value of IsChecked method, nothing would changed.
     Otherwise, the check box is set to checked or unchecked state, according to the value.
     */
    void SetIsChecked(bool is_checked) {
        if (IsChecked() != is_checked) {
            SetCheckState(is_checked ? CheckState::Checked : CheckState::Unchecked);
        }
    }

    /**
     Get the check state change event.
     */
    Observable<CheckStateChangedInfo> CheckStateChangedEvent() const;

protected:
    void Initialize() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    zaf::Rect DetermineTextRect() override;
    void OnClick(const ClickInfo& event_info) override;

private:
    void PaintBox(Canvas& canvas, const zaf::Rect& box_rect) const;

private:
    bool auto_check_{ true };
    bool can_be_indeterminate_{ false };
    zaf::CheckState check_state_;

    internal::ColorField box_back_color_field_;
    internal::ColorField box_border_color_field_;

    Event<CheckStateChangedInfo> check_state_changed_event_;
};

ZAF_OBJECT_BEGIN(CheckBox);
ZAF_OBJECT_PROPERTY(AutoCheck);
ZAF_OBJECT_PROPERTY(BoxBackColor);
ZAF_OBJECT_PROPERTY(BoxBackColorPicker);
ZAF_OBJECT_PROPERTY(BoxBorderColor);
ZAF_OBJECT_PROPERTY(BoxBorderColorPicker);
ZAF_OBJECT_PROPERTY(CanBeIndeterminate);
ZAF_OBJECT_PROPERTY(CheckState);
ZAF_OBJECT_PROPERTY(IsChecked);
ZAF_OBJECT_END;

}