#pragma once

#include <zaf/control/layout/axis_alignment.h>
#include <zaf/control/layout/control_alignment.h>
#include <zaf/control/layout/layout_direction.h>
#include <zaf/control/layout/layouter.h>

namespace zaf {

class LinearLayouter : public Layouter {
public:
	void SetDirection(LayoutDirection direction) {
		direction_ = direction;
	}

	void SetControlAlignment(ControlAlignment alignment) {
		control_alignment_ = alignment;
	}

	void SetAxisAlignment(AxisAlignment alignment) {
		axis_alignment_ = alignment;
	}

	void Layout(
		const Control& parent,
		const Rect& parent_old_rect,
		const std::vector<std::shared_ptr<Control>>& children) override;

private:
	LayoutDirection direction_{ LayoutDirection::LeftToRight };
	ControlAlignment control_alignment_{ ControlAlignment::Leading };
	AxisAlignment axis_alignment_{ AxisAlignment::Near };
};

}