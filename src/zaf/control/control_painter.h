#pragma once

#include <zaf/control/painter.h>

namespace zaf {

class ControlPainter : public Painter {
public:
	static const std::shared_ptr<ControlPainter>& GetInstance();

public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};

}