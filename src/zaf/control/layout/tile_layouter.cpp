#include <zaf/control/layout/tile_layouter.h>
#include <zaf/control/control.h>

namespace zaf {

static void LayoutWithTile(
	const std::shared_ptr<Control>& parent,
	const Rect& preivous_rect,
	const std::vector<std::shared_ptr<Control>>& children,
	bool is_vertical
);

const Layouter GetHorizontalTileLayouter() {
	return std::bind(LayoutWithTile, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false);
}


const Layouter GetVerticalTileLayouter() {
	return std::bind(LayoutWithTile, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, true);
}


static void LayoutWithTile(
	const std::shared_ptr<Control>& parent,
	const Rect& preivous_rect,
	const std::vector<std::shared_ptr<Control>>& children,
	bool is_vertical
) {

	if (children.empty()) {
		return;
	}

	Point children_position;
	Size children_size = parent->GetSize();

	if (is_vertical) {
		children_size.height /= children.size();
	}
	else {
		children_size.width /= children.size();
	}
	
	for (const auto& each_child : children) {

		each_child->SetRect(Rect(children_position, children_size));

		if (is_vertical) {
			children_position.y += children_size.height;
		}
		else {
			children_position.x += children_size.width;
		}
	}
}

}