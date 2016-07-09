#pragma once

#include <zaf/control/check_state.h>
#include <zaf/control/clickable_control.h>

namespace zaf {

/**
 Represents a check box control.	
 */
class CheckBox : public ClickableControl {
public:
	/**
	 Type of check state changed event.
	 */
	typedef Event<const std::shared_ptr<CheckBox>&> CheckStateChangeEvent;

public:
	/**
	 Initialize the instance.
	 */
	CheckBox();

	/**
	 Destroy the instance.
	 */
	~CheckBox();

	void Initialize() override;

	const Color GetBoxBorderColor() const {
		return GetBoxBorderColorPicker()(*this);
	}

	const ColorPicker GetBoxBorderColorPicker() const;

    void SetBoxBorderColor(const Color& color) {
        SetBoxBorderColorPicker(CreateColorPicker(color));
    }

	void SetBoxBorderColorPicker(const ColorPicker& color_picker);

    const Color GetBoxBackgroundColor() const {
        return GetBoxBackgroundColorPicker()(*this);
    }

    const ColorPicker GetBoxBackgroundColorPicker() const;

    void SetBoxBackgroundColor(const Color& color) {
        SetBoxBackgroundColorPicker(CreateColorPicker(color));
    }

    void SetBoxBackgroundColorPicker(const ColorPicker& color_picker);

	/**
	 Get a value indicating that whether the check box changes its check state when 
	 it is clicked.

	 The default value is true.
	 */
	bool CanAutoChangeCheckState() const;

	/**
	 Set a value indicating that whether the check box changes its check state when
	 it is clicked.
	 */
	void SetCanAutoChangeCheckState(bool can_change);

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
	CheckState GetCheckState() const {
		return check_state_;
	}

	/**
	 Set the check state of the check box.
	 */
	void SetCheckState(CheckState check_state);

	/**
	 Get a value indicating that whether the check box is checked.

	 @return
	 Return true if the check box is in either checked or indeterminate state; 
	 otherwise return false.

	 The default value is false.
	 */
	bool IsChecked() const {
		CheckState check_state = GetCheckState();
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
	CheckStateChangeEvent::Proxy GetCheckStateChangeEvent();

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    const Rect GetTextRect() const override;
	void MouseClick() override;

private:
	void PaintBox(Canvas& canvas, const Rect& box_rect) const;

private:
	CheckState check_state_;
};

}