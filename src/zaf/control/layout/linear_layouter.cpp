#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/control.h>
#include <zaf/control/layout/internal/array_layout_length_calculating.h>

namespace zaf {

void LinearLayouter::Layout(
	const Control& parent,
	const Rect& parent_old_rect,
	const std::vector<std::shared_ptr<Control>>& children
) {

	if (children.empty()) {
		return;
	}

	bool is_vertical = direction_ == Direction::TopToBottom;

	//Calcuate the length for each child.
	std::vector<internal::ArrayLayoutLengthCalculatingItem> calculate_items;
	calculate_items.reserve(children.size());

	for (const auto& each_child : children) {
		internal::ArrayLayoutLengthCalculatingItem item;
		item.minimum_length = is_vertical ? each_child->GetMinimumHeight() : each_child->GetMinimumWidth();
		item.maximum_length = is_vertical ? each_child->GetMaximumHeight() : each_child->GetMaximumWidth();
		calculate_items.push_back(item);
	}

	auto content_size = parent.GetContentSize();
	internal::CalculateArrayLayoutLengths(is_vertical ? content_size.height : content_size.width, calculate_items);

	//Record the position for each child.
	std::vector<float> expected_positions;
	expected_positions.reserve(calculate_items.size() + 1);

	float current_position = 0;
	for (const auto& each_item : calculate_items) {
		expected_positions.push_back(current_position);
		current_position += each_item.current_length;
	}
	expected_positions.push_back(current_position);

	//Set position and size to each child.
	float actual_position = 0;
	std::size_t index = 0;
	while (index < children.size()) {

		float actual_length = std::round(expected_positions[index + 1]) - std::round(expected_positions[index]);

		Rect new_rect;
		if (is_vertical) {
			new_rect = Rect(0, actual_position, content_size.width, actual_length);
		}
		else {
			new_rect = Rect(actual_position, 0, actual_length, content_size.height);
		}

		children[index]->SetRect(new_rect);

		actual_position += actual_length;
		++index;
	}
}

}