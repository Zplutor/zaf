#pragma once 

#include <zaf/base/event.h>
#include <zaf/control/clickable_control.h>

namespace zaf{

/**
 Represents a radio button conrol.	
 */
class RadioButton : public ClickableControl {
public:
	/**
	 Represents a radio button group.

	 Radio buttons associate with the same group cannot be selected at the same time.
	 Use RadioButton::SetGroup method to associate a radio buton with a group.
	 */
	class Group {
	public:
		/**
		 Get associated radio buttons.
		 */
		const std::vector<std::shared_ptr<RadioButton>> GetRadioButtons() const;

	private:
		friend class RadioButton;

		void Add(const std::shared_ptr<RadioButton>& radio_button);
		void Remove(const std::shared_ptr<RadioButton>& radio_button);

		void RadioButtonSelected(const std::shared_ptr<RadioButton>& radio_button);

	private:
		std::vector<std::weak_ptr<RadioButton>> radio_buttons_;
	};

	/**
	 Override. Add the radio paint component.
	 */
	class PaintComponent : public ClickableControl::PaintComponent {
	public:
		/**
		 Radio paint component.
		 */
		static const int Radio = ClickableControl::PaintComponent::Custom + 1;
	};

	/**
	 Type of select state change event.
	 */
	typedef Event<const std::shared_ptr<RadioButton>&> SelectStateChangeEvent;

public:
	/**
	 Construct the instance.
	 */
	RadioButton();

	/**
	 Destroy the instance.
	 */
	~RadioButton();

	/**
	 Get the associated group.

	 @return
	 Return nullptr if there is no associated group.
	 */
	const std::shared_ptr<Group>& GetGroup() const {
		return group_;
	}

	/**
	 Set the associated group.

	 The first radio button associates with a group would be selected.
	 The later associated radio buttons would be unselected.

	 If group is nullptr, the association would be removed.
	 */
	void SetGroup(const std::shared_ptr<Group>& group);

	/**
	 Get a value indicating whether the radio button changes its select state automatically 
	 when it is clicked.

	 The default value is true.
	 */
	bool CanAutoSelect() const;

	/**
	 Set a value indicating whether the radio button changes its select state automatically 
	 when it is clicked.
	 */
	void SetCanAutoSelect(bool can_auto_select);

	/**
	 Get a value indicating whether the radio button is selected.
	 */
	bool IsSelected() const {
		return is_selected_;
	}

	/**
	 Set the radio button selected.

	 Other radio buttons associate with the same group would be unselected.
	 */
	void SetSelected() {
		SetIsSelected(true);
	}

	/**
	 Get the select state change event.
	 */
	SelectStateChangeEvent::Proxy GetSelectStateChangeEvent();

protected:
	void Initialize() override;
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
	void MouseClick() override;

private:
	void PaintRadio(Canvas& canvas, const Rect& radio_rect);

	void SetUnselected() {
		SetIsSelected(false);
	}

	void SetIsSelected(bool is_selected);

private:
	bool is_selected_;
	std::shared_ptr<Group> group_;
};

}