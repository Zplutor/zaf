#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class CheckBox : public ClickableControl {
public:
	class PaintComponent : public ClickableControl::PaintComponent {
	public:
		static const int Box = ClickableControl::PaintComponent::Custom + 1;
	};

public:
	CheckBox();
	~CheckBox();

	const Color GetColor(int paint_component, int paint_state) const override;
	void SetColor(int paint_component, int paint_state, const Color& new_color) override;

	bool IsChecked() const {
		return is_checked_;
	}

	void SetIsChecked(bool is_check);

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
	void MouseClick() override;

private:
	std::wstring GetBoxColorPropertyName(int paint_state) const;

private:
	bool is_checked_;
};

}