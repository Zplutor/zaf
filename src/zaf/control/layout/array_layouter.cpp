#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/control.h>

namespace zaf {

static void LayoutWithArray(
	const std::shared_ptr<Control>& parent,
	const Rect& preivous_rect,
	const std::vector<std::shared_ptr<Control>>& children,
	bool is_vertical
);

const Layouter GetHorizontalArrayLayouter() {
	return std::bind(LayoutWithArray, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false);
}


const Layouter GetVerticalArrayLayouter() {
	return std::bind(LayoutWithArray, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, true);
}


static void LayoutWithArray(
	const std::shared_ptr<Control>& parent,
	const Rect& preivous_rect,
	const std::vector<std::shared_ptr<Control>>& children,
	bool is_vertical
) {

	if (children.empty()) {
		return;
	}

	std::vector<float> expected_positions;
	expected_positions.reserve(children.size() + 1);

	float expected_size = (is_vertical ? parent->GetHeight() : parent->GetWidth()) / children.size();
	for (std::size_t count = 0; count <= children.size(); ++count) {
		expected_positions.push_back(count * expected_size);
	}
	
	float actual_position = 0;
	std::size_t index = 0;
	while (index < children.size()) {

		float actual_size = std::round(expected_positions[index + 1]) - std::round(expected_positions[index]);

		Rect new_rect;
		if (is_vertical) {
			new_rect = Rect(0, actual_position, parent->GetWidth(), actual_size);
		}
		else {
			new_rect = Rect(actual_position, 0, actual_size, parent->GetHeight());
		}

		children[index]->SetRect(new_rect);

		actual_position += actual_size;
		++index;
	}
}

}