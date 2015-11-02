#pragma once

#include <zaf/control.h>

namespace zaf {

class Label : public Control {
protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}