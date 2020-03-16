#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/internal/linear_layout_length_calculating.h>

namespace zaf {

void LinearLayouter::Layout(
	const Control& parent,
	const Rect& parent_old_rect,
	const std::vector<std::shared_ptr<Control>>& children) {

	if (children.empty()) {
		return;
	}

	auto content_size = parent.GetContentSize();
	float content_length = IsVertical() ? content_size.height : content_size.width;

	//Calcuate length for each child.
	auto length_items = CalculateChildrenLengths(children, content_length);

	//Calculate position for each child according their lengths.
	auto children_positions = CalculateChildrenPositions(length_items);

	//Calculate position offset of the first child.
	float position_offset = CalculatePositionOffset(children_positions, content_length);

	//Set position and size to each child.
	SetRectsToChildren(children_positions, position_offset, content_size, children);
}


std::vector<internal::LinearLayoutLengthCalculatItem> LinearLayouter::CalculateChildrenLengths(
	const std::vector<std::shared_ptr<Control>>& children,
	float content_length) const {

	std::vector<internal::LinearLayoutLengthCalculatItem> result;
	result.reserve(children.size());

	for (const auto& each_child : children) {

		internal::LinearLayoutLengthCalculatItem item;

		item.minimum_length =
			IsVertical() ? each_child->GetMinimumHeight() : each_child->GetMinimumWidth();

		item.maximum_length =
			IsVertical() ? each_child->GetMaximumHeight() : each_child->GetMaximumWidth();

		result.push_back(item);
	}

	internal::CalculateArrayLayoutLengths(content_length, result);
	return result;
}


std::vector<float> LinearLayouter::CalculateChildrenPositions(
	const std::vector<internal::LinearLayoutLengthCalculatItem>& length_items) const {

	std::vector<float> result;
	result.reserve(length_items.size() + 1);

	float current_position = 0;
	for (const auto& each_item : length_items) {

		result.push_back(current_position);
		current_position += each_item.current_length;
	}

	result.push_back(current_position);
	return result;
}


float LinearLayouter::CalculatePositionOffset(
	const std::vector<float>& children_positions,
	float content_length) const {

	if (control_alignment_ == ControlAlignment::Leading) {
		return 0;
	}
	
	float total_length = children_positions.back() - children_positions.front();
	float space_length = content_length - total_length;

	if (control_alignment_ == ControlAlignment::Tailing) {
		return space_length;
	}

	if (control_alignment_ == ControlAlignment::Center) {
		return space_length / 2;
	}

	return 0;
}


void LinearLayouter::SetRectsToChildren(
	const std::vector<float>& children_positions,
	float position_offset,
	const Size& content_size,
	const std::vector<std::shared_ptr<Control>>& children) const {

	float actual_position = position_offset;

	for (std::size_t index = 0; index < children.size(); ++index) {

		float actual_length = children_positions[index + 1] - children_positions[index];

		Rect child_rect = GetChildRect(
			*children[index], 
			actual_position, 
			actual_length, 
			content_size);

		children[index]->SetRect(child_rect);

		actual_position += actual_length;
	}
}


Rect LinearLayouter::GetChildRect(
	const Control& child,
	float position,
	float length,
	const Size& content_size) const {

	Rect result;
	if (IsVertical()) {

		float width = (std::min)(content_size.width, child.GetMaximumWidth());
		width = (std::max)(width, child.GetMinimumWidth());
		result = Rect(
			CalculateAxisOffset(width, content_size.width),
			position,
			width,
			length);
	}
	else {

		float height = (std::min)(content_size.height, child.GetMaximumHeight());
		height = (std::max)(height, child.GetMinimumHeight());
		result = Rect(
			position,
			CalculateAxisOffset(height, content_size.height),
			length,
			height);
	}

	return result;
}


float LinearLayouter::CalculateAxisOffset(
	float control_length,
	float content_length) const {

	if (axis_alignment_ == AxisAlignment::Near) {
		return 0;
	}

	float space_length = content_length - control_length;

	if (axis_alignment_ == AxisAlignment::Far) {
		return space_length;
	}

	if (axis_alignment_ == AxisAlignment::Center) {
		return space_length / 2;
	}

	return 0;
}

}