#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class CheckBox : public ClickableControl {
public:
	CheckBox();
	~CheckBox();

	bool IsChecked() const {
		return is_checked_;
	}

	void SetIsChecked(bool is_check);

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
	void MouseClick() override;

private:
	bool is_checked_;
};

}