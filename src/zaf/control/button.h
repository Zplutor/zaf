#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class Button : public ClickableControl {
public:
	Button();
	~Button();

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}