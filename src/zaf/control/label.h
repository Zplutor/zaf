#pragma once

#include <zaf/control/control.h>

namespace zaf {

class Label : public Control {
public:
	Label();
	~Label();

protected:
	void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}