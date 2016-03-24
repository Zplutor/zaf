#pragma once 

#include <zaf/base/event.h>
#include <zaf/control/clickable_control.h>

namespace zaf{

class RadioButton : public ClickableControl {
public:
	class Group {
	public:
		const std::vector<std::shared_ptr<RadioButton>> GetRadioButtons() const;

	private:
		friend class RadioButton;

		void Add(const std::shared_ptr<RadioButton>& radio_button);
		void Remove(const std::shared_ptr<RadioButton>& radio_button);

		void RadioButtonSelected(const std::shared_ptr<RadioButton>& radio_button);

	private:
		std::vector<std::weak_ptr<RadioButton>> radio_buttons_;
	};

	class PaintComponent : public ClickableControl::PaintComponent {
	public:
		static const int Radio = ClickableControl::PaintComponent::Custom + 1;
	};

	typedef Event<const std::shared_ptr<RadioButton>&> SelectStateChangedEvent;

public:
	RadioButton();
	~RadioButton();

	const Color GetColor(int paint_component, int paint_state) const override;
	void SetColor(int paint_component, int paint_state, const Color& new_color) override;

	const std::shared_ptr<Group>& GetGroup() const {
		return group_;
	}

	void SetGroup(const std::shared_ptr<Group>& group);

	bool CanAutoSelect() const;
	void SetCanAutoSelect(bool can_auto_select);

	bool IsSelected() const {
		return is_selected_;
	}

	void SetSelected() {
		SetIsSelected(true);
	}

public:
	SelectStateChangedEvent::Proxy OnSelectStateChange;

protected:
	void Initialize() override;
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
	void MouseClick() override;

private:
	void PaintRadio(Canvas& canvas, const Rect& radio_rect);

	void SetUnselected() {
		SetIsSelected(false);
	}

	std::wstring GetRadioColorPropertyName(int paint_state) const;

	void SetIsSelected(bool is_selected);

private:
	bool is_selected_;
	std::shared_ptr<Group> group_;
	SelectStateChangedEvent select_state_change_event_;
};

}