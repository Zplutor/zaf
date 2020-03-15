#pragma once

#include <zaf/control/layout/axis_alignment.h>
#include <zaf/control/layout/control_alignment.h>
#include <zaf/control/layout/layout_direction.h>
#include <zaf/control/layout/layouter.h>

namespace zaf {
namespace internal {
class LinearLayoutLengthCalculatItem;
}

class Size;

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
	std::vector<internal::LinearLayoutLengthCalculatItem> CalculateChildrenLengths(
		const std::vector<std::shared_ptr<Control>>& children,
		float content_length) const;

	std::vector<float> CalculateChildrenPositions(
		const std::vector<internal::LinearLayoutLengthCalculatItem>& length_items) const;

	float CalculatePositionOffset(
		const std::vector<float>& children_positions,
		float content_length) const;

	void SetRectsToChildren(
		const std::vector<float>& children_positions,
		float position_offset,
		const Size& content_size,
		const std::vector<std::shared_ptr<Control>>& children) const;

	Rect GetChildRect(
		const Control& child,
		float position, 
		float length, 
		const Size& content_size) const;

	float CalculateAxisOffset(float control_length, float content_length) const;

	bool IsVertical() const {
		return direction_ == LayoutDirection::TopToBottom;
	}

private:
	LayoutDirection direction_{ LayoutDirection::LeftToRight };
	ControlAlignment control_alignment_{ ControlAlignment::Leading };
	AxisAlignment axis_alignment_{ AxisAlignment::Near };
};

}