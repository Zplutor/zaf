#pragma once

#include <zaf/control/check_state.h>
#include <zaf/control/clickable_control.h>

namespace zaf {

/**
 Represents a check box control.	
 */
class CheckBox : public ClickableControl {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
	/**
	 Type of check state changed event.
	 */
	typedef Event<const std::shared_ptr<CheckBox>&> CheckStateChangeEvent;

public:
	CheckBox();
	~CheckBox();

	void Initialize() override;

    /**
     Get the box border color.
     */
	Color GetBoxBorderColor() const {
		return GetBoxBorderColorPicker()(*this);
	}

    /**
     Get the color picker of the box border.
     */
	ColorPicker GetBoxBorderColorPicker() const;

    /**
     Set the box border color.
     */
    void SetBoxBorderColor(const Color& color) {
        SetBoxBorderColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of the box border.
     */
	void SetBoxBorderColorPicker(const ColorPicker& color_picker);

    /**
     Get the box background color.
     */
    Color GetBoxBackgroundColor() const {
        return GetBoxBackgroundColorPicker()(*this);
    }

    /**
     Get the color picker of the box background.
     */
    ColorPicker GetBoxBackgroundColorPicker() const;

    /**
     Set the box background color.
     */
    void SetBoxBackgroundColor(const Color& color) {
        SetBoxBackgroundColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of the box background.
     */
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
    Rect GetTextRect() override;
	void MouseClick() override;

    void DeserializeProperty(const std::wstring& name, const DataNode& data_node) override;

private:
	void PaintBox(Canvas& canvas, const Rect& box_rect) const;

private:
	CheckState check_state_;
};

}